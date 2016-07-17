/*
The MIT License (MIT)

Copyright (c) 2016 Oleg Linkin <maledictusdemagog@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

#include <memory>
#include <QObject>
#include <QPointer>
#include <QVariantMap>

namespace Mnemosy
{
class LJXmlRPC;
class UserProfile;

class MnemosyManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MnemosyManager)

    bool m_IsBusy;
    bool m_IsLogged;
    UserProfile *m_Profile;

    std::shared_ptr<LJXmlRPC> m_LJXmlPRC;

    Q_PROPERTY(bool busy READ GetBusy NOTIFY busyChanged)
    Q_PROPERTY(bool logged READ GetLogged NOTIFY loggedChanged)
    Q_PROPERTY(UserProfile* profile READ GetProfile NOTIFY profileChanged)

    explicit MnemosyManager(QObject *parent = 0);
public:
    static MnemosyManager* Instance(QObject *parent = 0);
    bool GetBusy() const;
    bool GetLogged() const;
    UserProfile* GetProfile() const;
private:
    void MakeConnections();

    void SetBusy(const bool busy);
    void SetLogged(const bool logged);
    void SetProfile(UserProfile *profile);

    void SetLogged(bool logged, const QString& login, const QString& password);

public slots:
    void login(const QString& login, const QString& password);
private slots:
    void handleLogin(bool success = false);

signals:
    void busyChanged();
    void loggedChanged();
    void profileChanged();
};
} // namespace Mnemosy
