#include "applicationsettings.h"

namespace Mnemosy
{
ApplicationSettings::ApplicationSettings(QObject *parent)
: MDConfAgent("/apps/mnemosy/application/", parent)
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
} // namespace Mnemosy

