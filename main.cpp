#include <QCoreApplication>
#include <QDebug>

#include "SettingsManager.h"

// -- Activte for testing factory defaults
#define EMPTY_SETTINGS_TEST

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

#ifndef EMPTY_SETTINGS_TEST
    const QString &ftpHostFake{"testServer"};
    constexpr quint32 ftpPortFake{2121};
    const QString& ftpUserFake{"admin"};
    const QString& ftpPWFake{"12345"};
#else
    const QString &ftpHostFake{};
    constexpr quint32 ftpPortFake{};
    const QString& ftpUserFake{};
    const QString& ftpPWFake{};
#endif

    // -- Save settings
    SettingsManager::save(KeyMapFTP::FtpServerHost, ftpHostFake);
    SettingsManager::save(KeyMapFTP::FtpServerPort, ftpPortFake);
    SettingsManager::save(KeyMapFTP::FtpServerUserName, ftpUserFake);
    SettingsManager::save(KeyMapFTP::FtpServerPassword, ftpPWFake);

    // -- Load settings
    const QString& newFtpHost = SettingsManager::load(KeyMapFTP::FtpServerHost).toString();
    const quint32 newFtpPort = SettingsManager::load(KeyMapFTP::FtpServerPort).toUInt();
    const QString& newFtpUser = SettingsManager::load(KeyMapFTP::FtpServerUserName).toString();
    const QString& newFtpPW = SettingsManager::load(KeyMapFTP::FtpServerPassword).toString();

    // -- Test logs
    qInfo() << "---------------------------------";
    qInfo() << "FTP Host:" << newFtpHost;
    qInfo() << "FTP Port:" << newFtpPort;
    qInfo() << "FTP User:" << newFtpUser;
    qInfo() << "FTP Password:" << newFtpPW;
    qInfo() << "---------------------------------";

    return QCoreApplication::exec();
}
