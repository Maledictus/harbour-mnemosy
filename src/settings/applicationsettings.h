#pragma once

#include <QObject>
#include <QVariant>

class MDConfGroup;

namespace Mnemosy
{
class ApplicationSettings : public QObject
{
    Q_OBJECT

    MDConfGroup *m_ApplicationGroup;

    ApplicationSettings(QObject *parent = 0);
public:
    ApplicationSettings(const ApplicationSettings& as) = delete;
    void operator=(const ApplicationSettings& as) = delete;

    static ApplicationSettings* Instance(QObject *parent = 0);

    Q_INVOKABLE QVariant value(const QString& key,
            const QVariant& def = QVariant()) const;
    Q_INVOKABLE void setValue(const QString& key, const QVariant& value);
};
} // namespace Mnemosy
