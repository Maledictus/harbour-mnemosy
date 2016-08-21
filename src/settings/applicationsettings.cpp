#include "applicationsettings.h"

#include <mlite5/MDConfGroup>

namespace Mnemosy
{
ApplicationSettings::ApplicationSettings(QObject *parent)
: QObject(parent)
, m_ApplicationGroup(new MDConfGroup("/apps/mnemosy/application", this))
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
    return m_ApplicationGroup ? m_ApplicationGroup->value(key, def) : def;
}

void ApplicationSettings::setValue(const QString& key, const QVariant& value)
{
    if (m_ApplicationGroup)
    {
        m_ApplicationGroup->setValue(key, value);
    }
}
} // namespace Mnemosy

