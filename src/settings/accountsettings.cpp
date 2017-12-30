#include "accountsettings.h"

#include <mlite5/MDConfGroup>

namespace Mnemosy
{
AccountSettings::AccountSettings(QObject *parent)
: QObject(parent)
, m_AccountGroup(new MDConfGroup("/apps/mnemosy/account", this))
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

QVariant AccountSettings::value(const QString& key, const QVariant& def) const
{
    return m_AccountGroup ? m_AccountGroup->value(key, def) : def;
}

void AccountSettings::setValue(const QString& key, const QVariant& value)
{
    if (m_AccountGroup)
    {
        m_AccountGroup->setValue(key, value);
    }
}

void AccountSettings::removeAll()
{
    if (m_AccountGroup)
    {
        m_AccountGroup->clear();
    }
}
} // namespace Mnemosy

