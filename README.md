# README

This example demonstrates how to implement a Settings Manager for handling all applications settings
in one place.

Can be used with Qt5 and Qt6.

## How it works

We have only two functions for save and load due to C++ template usage.
You have to define custom key maps and use them.

Assuming we have two key maps define (enums): KeyMapENV and KeyMapFS

    SettingsManager::save(KeyMapENV::PressureLevel, 123);
    QVariant value = SettingsManager::load(KeyMapENV::PressureLevel);
    
    SettingsManager::save(KeyMapFS::TempPath);
    QString value = SettingsManager::load(KeyMapFS::TempPath).toString;