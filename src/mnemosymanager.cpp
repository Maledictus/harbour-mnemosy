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
#include "src/models/ljfriendsgroupsmodel.h"
#include "src/settings/accountsettings.h"
#include "src/settings/applicationsettings.h"
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
, m_GroupsModel(new LJFriendGroupsModel(this))
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

LJFriendGroupsModel*MnemosyManager::GetGroupsModel() const
{
    return m_GroupsModel;
}

namespace
{
void PrepareSdelanoUNas(QString& event)
{
    QRegularExpression span("\\<span.+?\\>(.+?)<\\/span\\>(\\<br\\s*\\/\\>)*",
            QRegularExpression::CaseInsensitiveOption);
    event.replace(span, "\\1");
    QRegularExpression ul("\\<ul.+?(\\<img[\\w\\W]+?\\/\\>).+?\\/ul\\>",
            QRegularExpression::CaseInsensitiveOption);
    event.replace(ul, "\\1");
}
}

namespace
{
    struct ErrorCode2Message
    {
        QMap<int, QString> m_ErrorCode2Message;
        ErrorCode2Message()
        {
            m_ErrorCode2Message[100] = QObject::tr("Invalid username");
            m_ErrorCode2Message[101] = QObject::tr("Invalid password");
            m_ErrorCode2Message[102] = QObject::tr("Can't use custom/private security in communities");
            m_ErrorCode2Message[103] = QObject::tr("Poll error");
            m_ErrorCode2Message[104] = QObject::tr("Error adding one or more friends");
            m_ErrorCode2Message[105] = QObject::tr("Challenge expired");
            m_ErrorCode2Message[150] = QObject::tr("Can't post as non-user");
            m_ErrorCode2Message[151] = QObject::tr("Banned from journal");
            m_ErrorCode2Message[152] = QObject::tr("Can't post back-dated entries in a non-personal journal");
            m_ErrorCode2Message[153] = QObject::tr("Incorrent time value");
            m_ErrorCode2Message[154] = QObject::tr("Can't add a redirected account as a friend");
            m_ErrorCode2Message[155] = QObject::tr("Non-validated email address");
            m_ErrorCode2Message[156] = QObject::tr("Protocol authentication denied due to user's failure to accept TOS");
            m_ErrorCode2Message[157] = QObject::tr("Tags error");

            m_ErrorCode2Message[200] = QObject::tr("Missing required argument(s)");
            m_ErrorCode2Message[201] = QObject::tr("Unknown method");
            m_ErrorCode2Message[202] = QObject::tr("Too many arguments");
            m_ErrorCode2Message[203] = QObject::tr("Invalid argument(s)");
            m_ErrorCode2Message[204] = QObject::tr("Invalid metadata datatype");
            m_ErrorCode2Message[205] = QObject::tr("Unknown metadata");
            m_ErrorCode2Message[206] = QObject::tr("Invalid destination journal username");
            m_ErrorCode2Message[207] = QObject::tr("Protocol version mismatch");
            m_ErrorCode2Message[208] = QObject::tr("Invalid text encoding");
            m_ErrorCode2Message[209] = QObject::tr("Parameter out of range");
            m_ErrorCode2Message[210] = QObject::tr("Client tried to edit with corrupt data. Preventing");
            m_ErrorCode2Message[211] = QObject::tr("Invalid or malformed tag list");
            m_ErrorCode2Message[212] = QObject::tr("Message body is too long");
            m_ErrorCode2Message[213] = QObject::tr("Message body is empty");
            m_ErrorCode2Message[214] = QObject::tr("Message looks like spam");

            m_ErrorCode2Message[300] = QObject::tr("Don't have access to requested journal");
            m_ErrorCode2Message[301] = QObject::tr("Access of restricted feature");
            m_ErrorCode2Message[302] = QObject::tr("Can't edit post from requested journal");
            m_ErrorCode2Message[303] = QObject::tr("Can't edit post in this community");
            m_ErrorCode2Message[304] = QObject::tr("Can't delete post in this community");
            m_ErrorCode2Message[305] = QObject::tr("Action forbidden; account is suspended");
            m_ErrorCode2Message[306] = QObject::tr("This journal is temporarily in read-only mode. Try again in a couple minutes");
            m_ErrorCode2Message[307] = QObject::tr("Selected journal no longer exists");
            m_ErrorCode2Message[308] = QObject::tr("Account is locked and cannot be used");
            m_ErrorCode2Message[309] = QObject::tr("Account is marked as a memorial (journal is locked and does not accept comments)");
            m_ErrorCode2Message[310] = QObject::tr("Account user needs to be age-verified before use");
            m_ErrorCode2Message[311] = QObject::tr("Access temporarily disabled");
            m_ErrorCode2Message[312] = QObject::tr("Not allowed to add tags to entries in this journal");
            m_ErrorCode2Message[313] = QObject::tr("Must use existing tags for entries in this journal (can't create new ones)");
            m_ErrorCode2Message[314] = QObject::tr("Only paid users are allowed to use this request");
            m_ErrorCode2Message[315] = QObject::tr("User messaging is currently disabled");
            m_ErrorCode2Message[316] = QObject::tr("Poster is read-only and cannot post entries");
            m_ErrorCode2Message[317] = QObject::tr("Journal is read-only and entries cannot be posted to it");
            m_ErrorCode2Message[318] = QObject::tr("Poster is read-only and cannot edit entries");
            m_ErrorCode2Message[319] = QObject::tr("Journal is read-only and its entries cannot be edited");
            m_ErrorCode2Message[320] = QObject::tr("Sorry, there was a problem with entry content");
            m_ErrorCode2Message[321] = QObject::tr("Sorry, deleting is temporary disabled. Entry is private now");
            m_ErrorCode2Message[337] = QObject::tr("Not allowed to create comment");

            m_ErrorCode2Message[402] = QObject::tr("Your IP address has been temporarily banned for exceeding the login failure rate");
            m_ErrorCode2Message[404] = QObject::tr("Cannot post");
            m_ErrorCode2Message[405] = QObject::tr("Post frequency limit is exceeded");
            m_ErrorCode2Message[406] = QObject::tr("Client is making repeated requests. Perhaps it's broken?");
            m_ErrorCode2Message[407] = QObject::tr("Moderation queue full");
            m_ErrorCode2Message[408] = QObject::tr("Maximum queued posts for this community and poster combination reached");
            m_ErrorCode2Message[409] = QObject::tr("Post is too large");
            m_ErrorCode2Message[410] = QObject::tr("Your trial account has expired. Posting is now disabled");
            m_ErrorCode2Message[411] = QObject::tr("Action frequency limit is exceeded");

            m_ErrorCode2Message[500] = QObject::tr("Internal server error");
            m_ErrorCode2Message[501] = QObject::tr("Database error");
            m_ErrorCode2Message[502] = QObject::tr("Database is temporarily unavailable");
            m_ErrorCode2Message[503] = QObject::tr("Error obtaining necessary database lock");
            m_ErrorCode2Message[504] = QObject::tr("Protocol mode no longer supported");
            m_ErrorCode2Message[505] = QObject::tr("Account data format on server is old and needs to be upgraded");
            m_ErrorCode2Message[506] = QObject::tr("Journal sync is temporarily unavailable");
        }
    };

    QString GetLocalizedErrorMessage(int errorCode)
    {
        static ErrorCode2Message e2msg;
        if (!e2msg.m_ErrorCode2Message.contains(errorCode))
        {
            return QString();
        }

        return e2msg.m_ErrorCode2Message[errorCode];
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
                if (!success && !errorMsg.isEmpty())
                {
                    emit error(errorMsg, ETGeneral);
                }
            });
    connect(m_LJXmlPRC.get(),
            &LJXmlRPC::error,
            this,
            [=](const QString& msg, int code, ErrorType type)
            {
                SetBusy(false);
                const QString errorMessage = (type == ETLiveJournal ?
                        (tr("LiveJournal error (%1): ").arg(code) +
                                GetLocalizedErrorMessage(code)) :
                        msg);
                emit error(errorMessage, type);
            });
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
                emit friendsPageModelChanged();
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
                    LJEvent newEvent = event;
                    QString ev = newEvent.GetFullEvent();
                    Utils::RemoveStyleTag(ev);
                    PrepareSdelanoUNas(ev);
                    bool hasArg = false;
                    Utils::SetImagesWidth(ev, hasArg);
                    newEvent.SetFullHasArg(hasArg);
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
                    emit friendsPageModelChanged();
                }
            });
    connect(m_LJXmlPRC.get(),
            &LJXmlRPC::gotComments,
            this,
            [=](const LJPostComments& postComments)
            {
                m_CommentsModel->SetRawPostComments(postComments);
                m_CommentsModel->SetPostComments(postComments);
                emit commentsModelChanged();
            });
    connect(m_LJXmlPRC.get(),
            &LJXmlRPC::commentAdded,
            this,
            [=]()
            {
                qDebug() << "Comment added successfully";
                emit notify(tr("Comment was added"));
                //TODO add settings for refresh after comment adding
            });
    connect(m_LJXmlPRC.get(),
            &LJXmlRPC::commentEdited,
            this,
            [=]()
            {
                qDebug() << "Comment edited successfully";
                emit notify(tr("Comment was edited"));
                //TODO add settings for refresh after comment editing
            });
    connect(m_LJXmlPRC.get(),
            &LJXmlRPC::commentDeleted,
            this,
            [=]()
            {
                qDebug() << "Comment deleted successfully";
                emit notify(tr("Comment was deleted"));
                //TODO add settings for refresh after comment deleting
            });

    connect(m_LJXmlPRC.get(),
            &LJXmlRPC::gotFriendGroups,
            this,
            [=](const LJFriendGroups_t& groups)
            {
                if (m_Profile)
                {
                    m_Profile->SetFriendGroups(groups.toSet());
                    emit profileChanged();
                }
                m_GroupsModel->SetGroups(groups);
                emit groupsModelChanged();
            });
    connect(m_LJXmlPRC.get(),
            &LJXmlRPC::groupAdded,
            this,
            [=]()
            {
                qDebug() << "Group addede successfully";
                emit notify(tr("Friend group was added"));
                //TODO add settings for refresh after comment editing
            });
    connect(m_LJXmlPRC.get(),
            &LJXmlRPC::groupDeleted,
            this,
            [=]()
            {
                qDebug() << "Group deleted successfully";
                emit notify(tr("Friend group was deleted"));
                //TODO add settings for refresh after comment deleting
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

    if (m_Profile)
    {
        m_GroupsModel->SetGroups(m_Profile->GetFriendGroups().toList());
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
        ClearCache();
    }
    else
    {
        AccountSettings::Instance(this)->setValue("login", login);
        AccountSettings::Instance(this)->setValue("password", password);
    }
    SetLogged(logged);
}

void MnemosyManager::ClearCache()
{
    qDebug() << Q_FUNC_INFO;
    auto cacheDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QSettings settings(cacheDir + "/mnemosy_cache", QSettings::IniFormat);
    for (const auto& key : settings.allKeys())
    {
        settings.remove(key);
    }
    settings.sync();
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
    m_LJXmlPRC->GetFriendsPage(QDateTime::currentDateTime(),
            ApplicationSettings::Instance(this)->value("friendsPageFilter", 0)
                    .toInt());
}

void MnemosyManager::getNextFriendsPage()
{
    SetBusy(true);
    m_LJXmlPRC->GetFriendsPage(m_FriendsPageModel->GetLastItemPostDate(),
            ApplicationSettings::Instance(this)->value("friendsPageFilter", 0)
                   .toInt());
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

void MnemosyManager::editComment(const QString& journalName, quint64 dTalkId,
    const QString& subject, const QString& body)
{
    SetBusy(true);
    m_LJXmlPRC->EditComment(journalName, dTalkId, subject, body);
}

void MnemosyManager::deleteComment(const QString& journalName, quint64 dTalkId)
{
    SetBusy(true);
    m_LJXmlPRC->DeleteComment(journalName, dTalkId);
}

void MnemosyManager::getComments(quint64 dItemId, const QString& journal,
        int page, quint64 dTalkId)
{
    SetBusy(true);
    m_CommentsModel->Clear();
    m_LJXmlPRC->GetComments(dItemId, journal, page, dTalkId);
}

namespace
{
int GetFreeGroupId (const LJFriendGroups_t& groups)
{
    QVector<int> baseVector(30);
    int current = 0;
    std::generate(baseVector.begin(), baseVector.end(),
            [&current]() { return ++current; });

    QVector<int> existingIds;
    for (auto group : groups)
    {
        existingIds.append(group.GetId());
    }

    std::sort(existingIds.begin(), existingIds.end());
    QVector<int> result;
    std::set_difference(baseVector.begin(), baseVector.end(),
            existingIds.begin(), existingIds.end(),
            std::back_inserter(result));

    return result.value(0, -1);
}
}


void MnemosyManager::getFriendGroups()
{
    SetBusy(true);
    m_LJXmlPRC->GetFriendGroups();
}

void MnemosyManager::addFriendGroup(const QString& name, bool isPrivate)
{
    if (!m_Profile)
    {
        emit error(tr("Unable to add group"), ETGeneral);
        return;
    }

    const int id = GetFreeGroupId(m_Profile->GetFriendGroups().toList());
    if (id == -1)
    {
        qWarning () << Q_FUNC_INFO
                << "You cannot add more groups: the limit of 30 groups is reached";
        emit error(tr("The limit of 30 groups is reached. Delete old before adding new one"),
                ETGeneral);
        return;
    }
    SetBusy(true);
    m_LJXmlPRC->AddFriendGroup(name, isPrivate, id);
}

void MnemosyManager::deleteFriendGroup(quint64 groupId)
{
    SetBusy(true);
    m_LJXmlPRC->DeleteFriendGroup(groupId);
}

} // namespace Mnemosy
