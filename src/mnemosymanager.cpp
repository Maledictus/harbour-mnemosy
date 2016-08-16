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

#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include <QtDebug>
#include <QTimer>

#include "src/enumsproxy.h"
#include "src/lj-rpc/ljxmlrpc.h"
#include "src/models/ljcommentsmodel.h"
#include "src/models/ljeventsmodel.h"
#include "src/settings/accountsettings.h"
#include "src/userprofile.h"
#include "src/utils.h"


namespace Mnemosy
{
MnemosyManager::MnemosyManager(QObject *parent)
: QObject(parent)
, m_IsBusy(false)
, m_IsLogged(false)
, m_Profile(new UserProfile(this))
, m_FriendsPageModel(new LJEventsModel(this))
, m_CommentsModel(new LJCommentsModel(this))
, m_LJXmlPRC(new LJXmlRPC())
{
    MakeConnections();

    login(AccountSettings::Instance(this)->value("login", "").toString(),
            AccountSettings::Instance(this)->value("password", "").toString());
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

UserProfile* MnemosyManager::GetProfile() const
{
    return m_Profile;
}

LJEventsModel* MnemosyManager::GetFriendsPageModel() const
{
    return m_FriendsPageModel;
}

LJCommentsModel*MnemosyManager::GetCommentsModel() const
{
    return m_CommentsModel;
}

namespace
{
void PrepareSdelanoUNas(QString& event)
{
    QRegExp listRxp ("\\<ul\\s*style=\\\"list-style:\\s*.*;.*\\\"\\>"
            "\\<li\\>\\s*\\<!--noindex--\\>"
            "\\<a\\s*.*href=\\\".*sdelanounas\\.ru.*\\\".*\\>"
            "(\\<img[^\\>]*src\\s*=\\s*\"[^\"]*\"[^\\>]*\\/\\>)"
            "\\<\\/a\\>.*\\<\\/ul\\>", Qt::CaseInsensitive);
    listRxp.setMinimal(true);
    event.replace(listRxp, "\\1");
}
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
    connect(m_LJXmlPRC.get(),
            &LJXmlRPC::gotFriendsPage,
            this,
            [=](const LJEvents_t& events)
            {
                qDebug() << "Got friends events" << events.count();
                LJEvents_t newEvents = events;
                for (auto&& event : newEvents)
                {
                    bool hasArg = false;
                    QString ev = event.GetEvent();
                    Utils::SetImagesWidth(ev, hasArg);
                    PrepareSdelanoUNas(ev);
                    Utils::RemoveStyleTag(ev);
                    Utils::MoveFirstImageToTheTop(ev);

                    event.SetHasArg(hasArg);
                    event.SetEvent(ev);
                }
                m_FriendsPageModel->AddEvents(newEvents);
            });
    connect(m_LJXmlPRC.get(),
            &LJXmlRPC::gotEvent,
            this,
            [=](const LJEvent& event, ModelType mt)
            {
                qDebug() << "Got event";
                switch (mt)
                {
                case MTFeed:
                {
                    bool hasArg = false;
                    LJEvent newEvent = event;
                    QString ev = newEvent.GetFullEvent();
                    Utils::SetImagesWidth(ev, hasArg);
                    PrepareSdelanoUNas(ev);
                    Utils::RemoveStyleTag(ev);

                    newEvent.SetHasArg(hasArg);
                    newEvent.SetFullEvent(ev);

                    m_FriendsPageModel->UpdateEvent(newEvent);
                    emit gotEvent(m_FriendsPageModel->
                            GetEvent(newEvent.GetDItemID()).ToMap());
                }
                break;
                case MTMyBlog:
                break;
                case MTUserBlog:
                break;
                case MTUnknown:
                default:
                break;
                };
            });
    connect(m_LJXmlPRC.get(),
            &LJXmlRPC::commentsCountChanged,
            this,
            [=](quint64 dItemId, quint64 count)
            {
                LJEvent ev = m_FriendsPageModel->GetEvent(dItemId);
                if (ev.GetDItemID())
                {
                    ev.SetReplyCount(count);
                    m_FriendsPageModel->UpdateEvent(ev);
                }
            });
    connect(m_LJXmlPRC.get(),
            &LJXmlRPC::gotComments,
            this,
            [=](const LJPostComments& postComments)
            {
                m_CommentsModel->SetRawPostComments(postComments);
                m_CommentsModel->SetPostComments(postComments);
            });
    connect(m_LJXmlPRC.get(),
            &LJXmlRPC::commentAdded,
            this,
            [=]()
            {
                qDebug() << "Comment added successfully";
                //TODO add settings for refresh after comment adding
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
        AccountSettings::Instance(this)->setValue("login", login);
        AccountSettings::Instance(this)->setValue("password", password);
    }
    SetLogged(logged);
}

void MnemosyManager::CacheEvents()
{
    qDebug() << Q_FUNC_INFO;
    SaveItems("friends_page",
            m_FriendsPageModel->GetEvents().mid(0, LJXmlRPC::ItemShow));
}

void MnemosyManager::LoadCachedEvents()
{
    qDebug() << Q_FUNC_INFO;
    LoadItems("friends_page", m_FriendsPageModel);
}

void MnemosyManager::SaveItems(const QString& name, const LJEvents_t& events)
{
    qDebug() << Q_FUNC_INFO
            << name
            << "events count:" << events.count();
    auto cacheDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QDir dir(cacheDir);
    if (!dir.exists())
    {
        dir.mkpath(cacheDir);
    }

    QSettings settings(cacheDir + "/mnemosy_cache", QSettings::IniFormat);
    settings.beginWriteArray(name);
    for (int i = 0, size = events.size(); i < size; ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue("SerializedData", events.at(i).Serialize());
    }
    settings.endArray();
    settings.sync();
}

void MnemosyManager::LoadItems(const QString& name, LJEventsModel *model)
{
    qDebug() << Q_FUNC_INFO;
    auto path = QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
            "/mnemosy_cache";
    QSettings settings(path, QSettings::IniFormat);
    const int size = settings.beginReadArray(name);
    LJEvents_t events;
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        QByteArray data = settings.value("SerializedData").toByteArray();
        LJEvent event = LJEvent::Deserialize(data);
        if (!event.IsValid())
        {
            qWarning() << Q_FUNC_INFO
                    << "unserializable entry"
                    << i;
            continue;
        }
        events << event;
    }
    settings.endArray();

    if (model)
    {
        model->SetEvents(events);
    }
}

void MnemosyManager::abortRequest()
{
    m_LJXmlPRC->AbortRequest();
}

void MnemosyManager::login(const QString& login, const QString& password)
{
    if (login.isEmpty() || password.isEmpty())
    {
        return;
    }
    SetBusy(true);
    m_LJXmlPRC->Login(login, password);
}

void MnemosyManager::getFriendsPage()
{
    SetBusy(true);
    m_FriendsPageModel->Clear();
    m_LJXmlPRC->GetFriendsPage(QDateTime::currentDateTime());
}

void MnemosyManager::getNextFriendsPage()
{
    SetBusy(true);
    m_LJXmlPRC->GetFriendsPage(m_FriendsPageModel->GetLastItemPostDate());
}

void MnemosyManager::getEvent(quint64 dItemId, const QString& journalName,
        int modelType)
{
    SetBusy(true);
    m_LJXmlPRC->GetEvent(dItemId, journalName, static_cast<ModelType>(modelType));
}

void MnemosyManager::addComment(const QString& journalName, quint64 parentTalkId,
        quint64 dItemId, const QString& subject, const QString& body)
{
    SetBusy(true);
    m_LJXmlPRC->AddComment(journalName, parentTalkId, dItemId, subject, body);
}

void MnemosyManager::getComments(quint64 dItemId, const QString& journal,
        int page, quint64 dTalkId)
{
    SetBusy(true);
    m_CommentsModel->Clear();
    m_LJXmlPRC->GetComments(dItemId, journal, page, dTalkId);
}

} // namespace Mnemosy
