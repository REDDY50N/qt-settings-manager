#pragma once

#include <QMap>
#include <QString>
#include <QVariant>
#include <QMutexLocker>

// Switch between saved value and default
struct KeyConfig {
  QString key;
  QVariant defaultValue;
};

enum class KeyMapAPP {
  Theme, Language,
  // add more keys if needed ...
};


enum class KeyMapFTP {
  FtpServerHost, FtpServerPort, FtpServerPassword, FtpServerUserName,
  // add more keys if needed ...
};

enum class FactoryDefaultsFTP {
  FtpServerHost_Def, FtpServerPort_Def, FtpServerPassword_Def, FtpServerUserName_Def,
};

enum class FactoryDefaultsAPP {
  Theme_Def, Language_Def,
};



/*!
 *  @brief    Settings Manager for QSettings
 *
 * USAGE EXAMPLE:
 *
 *   // APP Settings:
 *   SettingsManager::save(PedCode, 123);
 *   QVariant value = SettingsManager::load(PedCode);
 *
 *   // PNET Settings:
 *   SettingsManager::save(TempProgramPath);
 *   QString value = SettingsManager::load(TempProgramPath).toString; */
class SettingsManager final {

public:
  // -- Save settings
  template <typename T>
  static bool save(T key, const QVariant &value, bool fallbackFile = false);

  // -- Load settings
  template <typename T> static QVariant load(T key);

  // -- KeyMaps (need to be public)
  static const QMap<KeyMapAPP, KeyConfig> keymapAPP;
  static const QMap<KeyMapFTP, KeyConfig> keymapFTP;
  // static const QMap<KeyMapAPP, QString> keymapAPP;
  // static const QMap<KeyMapFTP, QString> keymapFTP;


  static const QMap<FactoryDefaultsAPP, QVariant> factoryDefaultsAPP;
  static const QMap<FactoryDefaultsFTP, QVariant> factoryDefaultsFTP;

private:
  // -- Helper to get Key-String
  template <typename T> static QString getKeyString(T key);

  // -- Helper to get factory default key (if key value is empty)
  template <typename T> static QVariant getFactoryDefaultValue(T key);

  // -- Helpers for fallback  save/load
  static bool saveFallback(const QString &value, const QString &filename);
  static QString loadFallback(const QString &filename);

  // -- config path
  inline static const QString &m_configPathAPP{"/tmp/app.conf"};
  inline static const QString &m_configPathFTP{"/tmp/ftp.conf"};
  inline static const QString &m_fallbackPath{"/tmp/fallback/"};
};
