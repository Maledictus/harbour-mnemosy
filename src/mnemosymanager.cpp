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

#include "mnemosymanager.h"
#include <QtDebug>
#include <QTimer>

#include "src/lj-rpc/ljxmlrpc.h"
#include "src/settings/accountsettings.h"
#include "src/userprofile.h"


namespace Mnemosy
{
MnemosyManager::MnemosyManager(QObject *parent)
: QObject(parent)
, m_IsBusy(false)
, m_IsLogged(false)
, m_Profile(new UserProfile(this))
, m_LJXmlPRC(new LJXmlRPC())
{
    MakeConnections();
}

MnemosyManager* MnemosyManager::Instance(QObject *parent)
{
    static MnemosyManager *mnemosyManager = nullptr;
    if (!mnemosyManager)
    {
        mnemosyManager = new MnemosyManager(parent);
    }
    return mnemosyManager;
}

bool MnemosyManager::GetBusy() const
{
    return m_IsBusy;
}

bool MnemosyManager::GetLogged() const
{
    return m_IsLogged;
}

UserProfile*MnemosyManager::GetProfile() const
{
    return m_Profile;
}

void MnemosyManager::MakeConnections()
{
    connect(m_LJXmlPRC.get(),
            &LJXmlRPC::requestFinished,
            this,
            [=](bool success, const QString& errorMsg)
            {
                SetBusy(false);
                if (!success)
                {
                    //TODO show popup with errorMsg
                }
            });
//    connect(m_LJXmlPRC.get(),
//            &LJXmlRPC::ljError,
//            this,
//            [=](int code, const QString& msg)
//            {
//                SetBusy(false);
//                qDebug() << "Got livejournal error. Code:"
//                        << code
//                        << ", message:"
//                        << msg;
//                //TODO show popup with errorMsg
//            });
//    connect(m_LJXmlPRC.get(),
//            &LJXmlRPC::networkError,
//            this,
//            [=](int code, const QString& msg)
//            {
//                SetBusy(false);
//                qDebug() << "Got network error. Code:"
//                        << code
//                        << ", message:"
//                        << msg;
//                //TODO show popup with errorMsg
//            });
    connect(m_LJXmlPRC.get(),
            &LJXmlRPC::logged,
            this,
            [=](bool validated, const QString& login, const QString& password)
            {
                SetLogged(validated, login, password);
            });
    connect(m_LJXmlPRC.get(),
            &LJXmlRPC::gotUserProfile,
            this,
            [=](UserProfile *profile)
            {
                qDebug() << "Got profile";
                SetProfile(profile);
            });
}

void MnemosyManager::SetBusy(const bool busy)
{
    m_IsBusy = busy;
    emit busyChanged();
}

void MnemosyManager::SetLogged(const bool logged)
{
    m_IsLogged = logged;
    emit loggedChanged();
}

void MnemosyManager::SetProfile(UserProfile *profile)
{
    if (!m_Profile)
    {
        m_Profile = profile;
        m_Profile->setParent(this);
    }
    else if (profile)
    {
        m_Profile->UpdateProfile(profile);
        profile->deleteLater();
    }
    emit profileChanged();
}

void MnemosyManager::SetLogged(bool logged, const QString& login,
        const QString& password)
{
    qDebug() << Q_FUNC_INFO;
    if (!logged)
    {
        AccountSettings::Instance(this)->setValue("login", QVariant());
        AccountSettings::Instance(this)->setValue("password", QVariant());
        //TODO clear cache
    }
    else
    {
//        AccountSettings::Instance(this)->setValue("login", login);
//        AccountSettings::Instance(this)->setValue("password", password);
    }
    SetLogged(logged);
}

void MnemosyManager::login(const QString& login, const QString& password)
{
    SetBusy(true);
    m_LJXmlPRC->Login(login, password);
}

void MnemosyManager::handleLogin(bool success)
{
    bool res = true;
    SetBusy(false);
    SetLogged(res);
}
} // namespace Mnemosy