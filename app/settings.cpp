#include "settings.h"
#include <QSettings>
#include <QDebug>
#include <QFile>

#define SETTINGSFILE "/data/xochitl.conf"

Settings::Settings(QObject *parent) : QObject(parent)
{
}

void Settings::setValue(Settings::Key key, QVariant value)
{
#ifdef Q_PROCESSOR_ARM
    QSettings settings(SETTINGSFILE, QSettings::IniFormat);
    // FIXME: segfault
#else
    QSettings settings;
    settings.setValue(QVariant(key).toString(), value);
#endif
}

QVariant Settings::getValue(Settings::Key key, QVariant defaultValue)
{
#ifdef Q_PROCESSOR_ARM
    QSettings settings(SETTINGSFILE, QSettings::IniFormat);
#else
    QSettings settings;
#endif
    return settings.value(QVariant(key).toString(), defaultValue);
}

int Settings::thumbnailWidth()
{
    return 225;
}

int Settings::thumbnailHeight()
{
    return 300;
}

