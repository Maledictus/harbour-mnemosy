#pragma once

#include "src/dconf/mdconfagent.h"

namespace Mnemosy
{
class AccountSettings : public MDConfAgent
{
    Q_OBJECT

    AccountSettings(QObject *parent = 0);
public:
    AccountSettings(const AccountSettings& as) = delete;
    void operator=(const AccountSettings& as) = delete;

    static AccountSettings* Instance(QObject *parent = 0);
};
} // namespace Mnemosy
