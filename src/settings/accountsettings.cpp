#include "accountsettings.h"

namespace Mnemosy
{
AccountSettings::AccountSettings(QObject *parent)
: MDConfAgent("/apps/mnemosy/account/", parent)
{
}

AccountSettings* AccountSettings::Instance(QObject *parent)
{
    static AccountSettings *accSettings = nullptr;
    if (!accSettings)
    {
        accSettings = new AccountSettings(parent);
    }
    return accSettings;
}
} // namespace Mnemosy

