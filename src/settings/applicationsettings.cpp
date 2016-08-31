#include "applicationsettings.h"

#include <QStandardPaths>

namespace Mnemosy
{
ApplicationSettings::ApplicationSettings(QObject *parent)
: QObject(parent)
, m_Settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
            "/harbour-mnemosy.conf",
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
} // namespace Mnemosy

