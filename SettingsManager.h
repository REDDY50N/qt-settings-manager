#pragma once

#include <QVariant>

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



class SettingsManager final {

public:
  /// Save settings interface
  template <typename T>
  static bool save(T key, const QVariant &value, bool fallbackFile = false);

  /// Load settings interface
  template <typename T> static QVariant load(T key);

  /// KeyMaps (need to be public)
  static const QMap<KeyMapAPP, KeyConfig> keymapAPP;
  static const QMap<KeyMapFTP, KeyConfig> keymapFTP;

private:
  /// Helper to get Key-String
  template <typename T> static QString getKeyString(T key); // HINT: Add each new map in definiton

  /// Load the matching config file for the given map
  template <typename T> static QString getConfigFilePath(); // HINT: Add each new map in definiton

  /// Helpers for fallback  save/load
  static bool saveFallback(const QString &value, const QString &filename);
  static QString loadFallback(const QString &filename);

  /// Config file path (QSettings)
  inline static const QString &m_configPathAPP{"/tmp/app.conf"};
  inline static const QString &m_configPathFTP{"/tmp/ftp.conf"};

  /// Single fallback values will be saved in /path/to/fallback/<key>
  inline static const QString &m_fallbackPath{"/tmp/fallback/"};
};
