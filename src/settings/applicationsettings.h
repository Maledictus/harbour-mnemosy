#pragma once

#include "src/dconf/mdconfagent.h"

namespace Mnemosy
{
class ApplicationSettings : public MDConfAgent
{
    Q_OBJECT
    Q_DISABLE_COPY(ApplicationSettings)

    ApplicationSettings(QObject *parent = 0);
public:
    static ApplicationSettings* Instance(QObject *parent = 0);
};
} // namespace Mnemosy
