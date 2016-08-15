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
#include <QRegularExpression>
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
void PrepareImages(QString& event, bool& hasArg)
{
    QRegExp imgRxp ("\\<img[^\\>]*src\\s*=\\s*\"[^\"]*\"[^\\>]*\\>",
            Qt::CaseInsensitive);
    imgRxp.setMinimal(true);
    int offset = 0;
    QList<std::tuple<QString, QString, int>> matched;
    while ((offset = imgRxp.indexIn(event, offset)) != -1)
    {
        QString imgTag = imgRxp.cap(0);
        if (!imgTag.contains("l-stat.livejournal.net"))
        {
            QRegExp urlRxp("src\\s*=\\s*[\"']([^\"]*)[\"']");
            QString url;
            if (urlRxp.indexIn(imgTag) != -1)
                url = urlRxp.cap(1);
            int width = 0;
            QRegExp widthRxp("width\\s*=\\s*[\"'](\\d+)[\"']");
            if (widthRxp.indexIn(imgTag) != -1)
                width = widthRxp.cap(1).toInt ();

            matched << std::make_tuple(imgTag, url, width);
        }
        offset += imgRxp.matchedLength();
    }

    for (const auto& t : matched)
    {
        event.replace (std::get<0>(t),
                "<img src=\"" + std::get<1>(t) + QString("\" width=\"%1\" />"));
        hasArg = true;
    }
}

void PrepareStyle(QString& event)
{
    QRegularExpression styleRxp ("style=\"(.+?)\"",
            QRegularExpression::CaseInsensitiveOption);
    event.remove(styleRxp);
}

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

void PrepareFirstImagePosition(QString& event)
{
    QRegularExpression imgRxp("(\\<img[\\w\\W]+?/\\>)",
            QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = imgRxp.match(event);
    if (match.hasMatch())
    {
        QString matched = match.captured(0);
        event.remove(match.capturedStart(0), match.capturedLength(0));
        event.push_front(matched);
    }
}

void PrepareComment(LJComment& comment)
{
    bool hasArg = false;
    QString body = comment.GetBody();
    PrepareImages(body, hasArg);
    comment.SetHasArgs(hasArg);
    comment.SetBody(body);
}

LJComments_t ExpandComment(LJComment& comment, int level)
{
    LJComments_t comments;

    LJComments_t& children = comment.GetChildrenRef();
    for (int i = 0; i < children.count(); ++i)
    {
        LJComment child = children.at(i);
        if (child.GetLevel() - level < 4)
        {
            PrepareComment(child);
            comments << child;
            children.takeAt(i--);
            if (child.GetChildrenCount() > 0)
            {
                comments << ExpandComment(comments.last(), level);
            }
        }
    }

    return comments;
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
                    PrepareImages(ev, hasArg);
                    PrepareSdelanoUNas(ev);
                    PrepareStyle(ev);
                    PrepareFirstImagePosition(ev);

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
                    PrepareImages(ev, hasArg);
                    PrepareSdelanoUNas(ev);
                    PrepareStyle(ev);

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
            &LJXmlRPC::gotComments,
            this,
            [=](const LJPostComments& postComments)
            {
                m_CommentsModel->SetRawPostComments(postComments);

                LJComments_t list;
                for (const auto& comment : postComments.m_Comments)
                {
                    LJComment tempComment = comment;
                    PrepareComment(tempComment);
                    list << tempComment;
                    if (tempComment.GetChildrenCount() > 0)
                    {
                        list << ExpandComment(list.last(), list.last().GetLevel());
                    }
                }
                LJPostComments comms = postComments;
                comms.m_Comments = list;
                m_CommentsModel->SetPostComments(comms);
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
