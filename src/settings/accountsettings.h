#pragma once

#include <QObject>
#include <QVariant>

class MDConfGroup;

namespace Mnemosy
{
class AccountSettings : public QObject
{
    Q_OBJECT

    MDConfGroup *m_AccountGroup;

    AccountSettings(QObject *parent = 0);
public:
    AccountSettings(const AccountSettings& as) = delete;
    void operator=(const AccountSettings& as) = delete;

    static AccountSettings* Instance(QObject *parent = 0);

    Q_INVOKABLE QVariant value(const QString& key,
            const QVariant& def = QVariant()) const;
    Q_INVOKABLE void setValue(const QString& key, const QVariant& value);
};
} // namespace Mnemosy
