#include "SettingsManager.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSettings>

// ----------------------------------------------
// KEY MAP - APP
// ----------------------------------------------
const QMap<KeyMapAPP, KeyConfig> SettingsManager::keymapAPP = {
  { KeyMapAPP::Theme, { "App/Theme", "Light" } },
  { KeyMapAPP::Language, { "App/Language", "German" } }
};

// ----------------------------------------------
// KEY MAP - FTP
// ----------------------------------------------
const QMap<KeyMapFTP, KeyConfig> SettingsManager::keymapFTP = {
  { KeyMapFTP::FtpServerHost, { "FTP/ServerHost", "ftp.dlptest.com" } },
  { KeyMapFTP::FtpServerPort, { "FTP/ServerPort", 21 } },
  { KeyMapFTP::FtpServerUserName, { "FTP/UserName", "dlpuser" } },
  { KeyMapFTP::FtpServerPassword, { "FTP/Password", "rNrKYTX9g7z3RgJRmxWuGHbeu" } }
};


// ----------------------------------------------
// SAVE SETTINGS
// ----------------------------------------------
template <typename T>
bool SettingsManager::save(T key, const QVariant& value, const bool fallbackFile) {

  // -- Check if a valid keymap was passed
  // HINT: Maybe use a helper function if using many keys maps
  static_assert(std::is_same_v<T, KeyMapFTP> || std::is_same_v<T, KeyMapAPP>, "SETTINGS: Invalid key type");

  // -- Invoke helper to get key string from map
  const QString keyString = getKeyString(key);

  // -- Load the config file path for this map
  const QString configFilePath = getConfigFilePath<T>();

  // -- Initialize QSettings
  // HINT: Native - Store the settings in a platform dependent location / Ini - Store the settings in a text file
  QSettings settings{ configFilePath, QSettings::IniFormat };

  // -- Empty key error check
  if (keyString.isEmpty()) {
    qWarning() << "SETTINGS: Invalid key! Does not exists in settings keymap! Key:" << keyString;
    return false;
  }

  // -- Check if the current value is different from the new value
  if (settings.value(keyString) != value) {

    // -- If different, save the new value
    settings.setValue(keyString, value);

    // -- Debug message
    qInfo() << "SETTINGS: Saved Key:" << keyString << " Value:" << settings.value(keyString).toString() << "File:" << configFilePath;

    // -- Save as fallback in file (keyname = filename)
    if (fallbackFile)
      saveFallback(value.toString(), keyString.mid(keyString.indexOf("/")));
  } else {
    qInfo() << "SETTINGS: No change for Key:" << keyString << "Save fallback:" << fallbackFile;
  }

  // -- return if no error
  return settings.status() == QSettings::NoError;
}

// ----------------------------------------------
// LOAD SETTINGS
// ----------------------------------------------
template <typename T>
QVariant SettingsManager::load(T key) {

  // -- Check if a valid keymap was passed
  // HINT: Maybe use a helper function if using many keys maps
  static_assert(std::is_same_v<T, KeyMapFTP> || std::is_same_v<T, KeyMapAPP>, "SETTINGS: Invalid key type");

  // -- Invoke helper to get key string from map
  const QString& keyString = getKeyString(key);

  // -- Load the config file path for this map
  const QString configFilePath = getConfigFilePath<T>();

  // -- Initialize QSettings
  const QSettings settings{ configFilePath, QSettings::IniFormat };

  // -- Check if key exists and is not empty
  QVariant value = settings.value(keyString);

  // -- Load fallback if empty
  if (value.isNull() || value.toString().isEmpty()) {
    qWarning() << "SETTINGS: Key" << keyString << "not found or value is empty. Try to load fallback!";

    // -- Try to load fallback
    const QString fallbackValue = loadFallback(keyString.mid(keyString.indexOf("/") + 1));

    // -- Convert to QVariant (for consistance=
    value = QVariant(fallbackValue);

    // -- Use factory default (last option)
    if (value.isNull() || value.toString().isEmpty()) {
      qWarning() << "SETTINGS: Key" << keyString << "not found. Using default value.";

      if constexpr (std::is_same_v<T, KeyMapAPP>) {
        value = keymapAPP.value(key).defaultValue;
      } else if constexpr (std::is_same_v<T, KeyMapFTP>) {
        value = keymapFTP.value(key).defaultValue;
      }
    }
  }


  qInfo() << "SETTINGS: Loaded Key:" << keyString << "Value:" << value.toByteArray() << "File:" << configFilePath;
  // ------------------------------------------

  // -- Return values as QVariant
  return value;
}

// ----------------------------------------------
// HELPER - GET KEY STRING
// ----------------------------------------------
template <typename T>
QString SettingsManager::getKeyString(T key) {
  if constexpr (std::is_same_v<T, KeyMapFTP>) {
    return keymapFTP.value(key).key;
  } else if constexpr (std::is_same_v<T, KeyMapAPP>) {
    return keymapAPP.value(key).key;
  }

  return {}; // Fallback: Should not be reached
}

// ----------------------------------------------
// HELPER - GET QSETTINGS CONFIG FILE PATH
// ----------------------------------------------
template <typename TYPE>
QString SettingsManager::getConfigFilePath() {
  static_assert(std::is_same_v<TYPE, KeyMapAPP> || std::is_same_v<TYPE, KeyMapFTP>, "SETTINGS: Invalid keymap for config path");

  if constexpr (std::is_same_v<TYPE, KeyMapAPP>) {
    return m_configPathAPP;
  } else if constexpr (std::is_same_v<TYPE, KeyMapFTP>) {
    return m_configPathFTP;
  }

  return{}; // Fallback: Should not be reached
}

// ----------------------------------------------
// HELPER - SAVE FALLBACK
// ----------------------------------------------
bool SettingsManager::saveFallback(const QString& value, const QString& filename) {
  // -- Write value into fallback file
  const QDir dir(m_fallbackPath);
  if (!dir.exists() && !dir.mkpath(".")) {
    qWarning() << "SETTINGS: Could not create default fallback dir:" << m_fallbackPath;
    return false;
  }

  // -- Check path exists
  QFile file(dir.filePath(m_fallbackPath + filename));
  if (!file.open(QIODevice::WriteOnly)) {
    qWarning() << "SETTINGS: Could not open file to write:" << m_fallbackPath + filename;
    return false;
  }

  qInfo() << "SETTINGS: Fallback value saved:" << value << QString("File: %1.conf").arg(m_fallbackPath + filename);
  // qInfo() << "SETTINGS: Saving into fallback file:" << filename << "Data:" <<
  // value;
  file.write(value.toUtf8().trimmed());
  file.close();
  return true;
}

// ----------------------------------------------
// HELPER - LOAD FALLBACK
// ----------------------------------------------
QString SettingsManager::loadFallback(const QString& filename) {
  const QString filepath = m_fallbackPath + filename;
  QFile file(filepath);

  if (!file.open(QIODevice::ReadOnly)) {
    // TODO: Check only fallback option was set
    qWarning() << "SETTINGS: Could not open file to read:" << filepath << "Error:" << file.errorString();
    return "";
  }

  QString value = file.readAll();
  file.close();

  qInfo() << "SETTINGS: Fallback value loaded:" << value << "Key:" << filename << QString("File: %1.conf").arg(filename);

#ifdef WRITE_TO_QSETTINGS_FILE
  QSettings settings("/data/config/defaults/config.ini", QSettings::IniFormat);
  settings.setValue("defaultCode", value);
#endif

  return value;
}

// ----------------------------------------------
// EXPLICIT TEMPLATE INSTANTIATION
// ----------------------------------------------
// Explicit instantiation to make the Linker happy by avoiding undefined reference errors
// Needs to be done for each key map with load and save function
//
template bool SettingsManager::save<KeyMapFTP>(KeyMapFTP, const QVariant&, bool);
template bool SettingsManager::save<KeyMapAPP>(KeyMapAPP, const QVariant&, bool);

template QVariant SettingsManager::load<KeyMapFTP>(KeyMapFTP);
template QVariant SettingsManager::load<KeyMapAPP>(KeyMapAPP);
