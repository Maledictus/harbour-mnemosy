#include "applicationsettings.h"

#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>

namespace Mnemosy
{
ApplicationSettings::ApplicationSettings(QObject *parent)
: QObject(parent)
, m_Settings(QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
             .filePath(QCoreApplication::applicationName()) + "/mnemosy.conf",
        QSettings::IniFormat)
{
}

ApplicationSettings* ApplicationSettings::Instance(QObject *parent)
{
    static ApplicationSettings *appSettings = nullptr;
    if (!appSettings)
    {
        appSettings = new ApplicationSettings(parent);
    }
    return appSettings;
}

QVariant ApplicationSettings::value(const QString& key, const QVariant& def) const
{
    return m_Settings.value(key, def);
}

void ApplicationSettings::setValue(const QString& key, const QVariant& value)
{
    m_Settings.setValue(key, value);
}

void ApplicationSettings::remove(const QString& key)
{
    m_Settings.remove(key);
}
} // namespace Mnemosy

