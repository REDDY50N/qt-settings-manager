#include "SettingsManager.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSettings>

// ----------------------------------------------
// KEY MAP - APP
// ----------------------------------------------
const QMap<KeyMapAPP, QString> SettingsManager::keymapAPP = {
    {KeyMapAPP::Theme, "App/Theme"},
  {KeyMapAPP::Language, "App/Language"}};

// ----------------------------------------------
// KEY MAP - FTP
// ----------------------------------------------
const QMap<KeyMapFTP, QString> SettingsManager::keymapFTP = {
    {KeyMapFTP::FtpServerHost, "FTP/ServerHost"},
    {KeyMapFTP::FtpServerPort, "FTP/ServerPort"},
    {KeyMapFTP::FtpServerUserName, "FTP/UserName"},
    {KeyMapFTP::FtpServerPassword, "FTP/Password"},
};

// ----------------------------------------------
// FACTORY DEFAULTS - APP
// ----------------------------------------------
const QMap<FactoryDefaultsAPP, QVariant> SettingsManager::factoryDefaultsAPP = {
    {FactoryDefaultsAPP::Theme_Def, "Light"},
    {FactoryDefaultsAPP::Language_Def, "German"}};

// ----------------------------------------------
// FACTORY DEFAULTS - FTP
// ----------------------------------------------
const QMap<FactoryDefaultsFTP, QVariant> SettingsManager::factoryDefaultsFTP = {
    {FactoryDefaultsFTP::FtpServerHost_Def, "ftp.dlptest.com"},
    {FactoryDefaultsFTP::FtpServerPort_Def, 21},
    {FactoryDefaultsFTP::FtpServerUserName_Def, "dlpuser"},
    {FactoryDefaultsFTP::FtpServerPassword_Def,
     "rNrKYTX9g7z3RgJRmxWuGHbeu"}, // HINT: paddword should by decoded .i.e. as
                                   // QByteArray
};

// ----------------------------------------------
// SAVE SETTINGS
// ----------------------------------------------
template <typename T>
bool SettingsManager::save(T key, const QVariant &value,
                           const bool fallbackFile) {
  static_assert(std::is_same_v<T, KeyMapFTP> || std::is_same_v<T, KeyMapAPP>,
                "SETTINGS: Invalid key type");

  const QString keyString = getKeyString(key);
  // qInfo() << "SETTINGS:" << keyString << fallbackFile << value;

  // -- Load the given config file path
  QString configFilePath{};
  if constexpr (std::is_same_v<T, KeyMapAPP>) {
    configFilePath = m_configPathAPP;
  } else if constexpr (std::is_same_v<T, KeyMapFTP>) {
    configFilePath = m_configPathFTP;
  }

  // -- Initialize QSettings
  // HINT: Native - Store the settings in a platform dependent location / Ini -
  // Store the settings in a text file
  QSettings settings{configFilePath, QSettings::IniFormat};

  // -- Empty key error check
  if (keyString.isEmpty()) {
    qWarning()
        << "SETTINGS: Invalid key! Does not exists in settings keymap! Key:"
        << keyString;
    return false;
  }

  // -- Check if the current value is different from the new value
  if (settings.value(keyString) != value) {

    // -- If different, save the new value
    settings.setValue(keyString, value);

    // -- Debug message
    qInfo() << "SETTINGS: Saved Key:" << keyString
            << " Value:" << settings.value(keyString).toString()
            << "File:" << configFilePath;

    // -- Save as fallback in file (keyname = filename)
    if (fallbackFile)
      saveFallback(value.toString(), keyString.mid(keyString.indexOf("/")));
  } else {
    qInfo() << "SETTINGS: No change for Key:" << keyString
            << "Save fallback:" << fallbackFile;
  }

  // -- return if no error
  return settings.status() == QSettings::NoError;
}

// ----------------------------------------------
// LOAD SETTINGS
// ----------------------------------------------
template <typename T> QVariant SettingsManager::load(T key) {
  static_assert(std::is_same_v<T, KeyMapFTP> || std::is_same_v<T, KeyMapAPP>,
                "SETTINGS: Invalid key type");

  // -- Get the keymap's key string
  const QString& keyString = getKeyString(key);

  // -- Set config file path depending on KeyMap
  QString configFilePath{};
  if constexpr (std::is_same_v<T, KeyMapAPP>) {
    configFilePath = m_configPathAPP;
  } else if constexpr (std::is_same_v<T, KeyMapFTP>) {
    configFilePath = m_configPathFTP;
  }

  // -- Initialize QSettings
  const QSettings settings{configFilePath, QSettings::IniFormat};

  // -- Check if key exists and is not empty
  QVariant value = settings.value(keyString);

  // -- Load fallback if empty
  if (value.isNull() || value.toString().isEmpty()) {
    // const QString fallbackValue = loadFallback(key);
    qWarning() << "SETTINGS: Key" << keyString
               << "not found or value is empty. Try to load fallback!";
    const QString fallbackValue =
        loadFallback(keyString.mid(keyString.indexOf("/") + 1));

    // -- Convert to QVariant for consitency
    value = QVariant(fallbackValue);

    // -- Use factory default there is no fallback
    if (value.isNull() || value.toString().isEmpty()) {

      const QString& keyStringDef = getFactoryDefaultValue(key+ "_Def");
      value = settings.value(keyStringDef);

      qWarning()
          << "SETTINGS: No valid settings found. Using factory default - Key:"
          << keyString << "Value:" << value;
    }
  }

  qInfo() << "SETTINGS: Loaded Key:" << keyString
          << "Value:" << value.toByteArray() << "File:" << configFilePath;
  // ------------------------------------------

  // -- Return values as QVariant
  return value;
}

// ----------------------------------------------
// HELPER - GET KEY STRING
// ----------------------------------------------
template <typename T> QString SettingsManager::getKeyString(T key) {
  if constexpr (std::is_same_v<T, KeyMapFTP>) {
    return keymapFTP.value(key);
  } else if constexpr (std::is_same_v<T, KeyMapAPP>) {
    return keymapAPP.value(key);
  }

  // -- Fallback: Should not be reached
  return {};
}

// ----------------------------------------------
// HELPER - GET FACTORY DEFAULT VALUE
template <typename T> QString SettingsManager::getFactoryDefaultValue(T key) {
  if constexpr (std::is_same_v<T, FactoryDefaultsFTP>) {
    qWarning() << "SETTINGS: Load factory defaults for key:" << key << "Value:" << factoryDefaultsFTP.value(key);
    return factoryDefaultsFTP.value(key);
  } else if constexpr (std::is_same_v<T, FactoryDefaultsAPP>) {
    qWarning() << "SETTINGS: Load factory defaults for key:" << key <<"Value:" << factoryDefaultsAPP.value(key);
    return factoryDefaultsAPP.value(key);
  }

  // -- Fallback: Should not be reached
  return {};
}

// ----------------------------------------------
// HELPER - SAVE FALLBACK
// ----------------------------------------------
bool SettingsManager::saveFallback(const QString &value,
                                   const QString &filename) {
  // -- Write value into fallback file
  const QDir dir(m_fallbackPath);
  if (!dir.exists() && !dir.mkpath(".")) {
    qWarning() << "SETTINGS: Could not create default fallback dir:"
               << m_fallbackPath;
    return false;
  }

  // -- Check path exists
  QFile file(dir.filePath(m_fallbackPath + filename));
  if (!file.open(QIODevice::WriteOnly)) {
    qWarning() << "SETTINGS: Could not open file to write:"
               << m_fallbackPath + filename;
    return false;
  }

  qInfo() << "SETTINGS: Fallback value saved:" << value
          << QString("File: %1.conf").arg(m_fallbackPath + filename);
  // qInfo() << "SETTINGS: Saving into fallback file:" << filename << "Data:" <<
  // value;
  file.write(value.toUtf8().trimmed());
  file.close();
  return true;
}

// ----------------------------------------------
// HELPER - LOAD FALLBACK
// ----------------------------------------------
QString SettingsManager::loadFallback(const QString &filename) {
  const QString filepath = m_fallbackPath + filename;
  QFile file(filepath);

  if (!file.open(QIODevice::ReadOnly)) {
    // TODO: Check only fallback option was set
    qWarning() << "SETTINGS: Could not open file to read:" << filepath
               << "Error:" << file.errorString();
    return "";
  }

  QString value = file.readAll();
  file.close();

  qInfo() << "SETTINGS: Fallback value loaded:" << value << "Key:" << filename
          << QString("File: %1.conf").arg(filename);

#ifdef WRITE_TO_QSETTINGS_FILE
  QSettings settings("/data/config/defaults/config.ini", QSettings::IniFormat);
  settings.setValue("defaultCode", value);
#endif

  return value;
}

// ----------------------------------------------
// EXPLICIT INSTANTIATION FOR PNET & APP TEMPLATE
// ----------------------------------------------
// Make the Linker happy, avoid errors like:
// undefined reference to `bool SettingsManager::save<KeyMapFTP>(KeyMapFTP,
// QVariant const&, bool)` Explizite Instanziierung für KeyMapFTP und KeyMapAPP
// Die Fehler entstehen, weil die Template-Implementierungen in der .cpp-Datei
// nicht für die verwendeten Typen (KeyMapFTP, KeyMapAPP) instanziiert werden.
// Durch eine der beiden Lösungen wird sichergestellt, dass der Linker die
// benötigten Funktionen findet.

template bool SettingsManager::save<KeyMapFTP>(KeyMapFTP, const QVariant &,
                                               bool);
template QVariant SettingsManager::load<KeyMapFTP>(KeyMapFTP);
template QString SettingsManager::getKeyString<KeyMapFTP>(KeyMapFTP);

template bool SettingsManager::save<KeyMapAPP>(KeyMapAPP, const QVariant &,
                                               bool);
template QVariant SettingsManager::load<KeyMapAPP>(KeyMapAPP);
template QString SettingsManager::getKeyString<KeyMapAPP>(KeyMapAPP);