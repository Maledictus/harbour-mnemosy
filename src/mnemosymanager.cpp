/*
The MIT License (MIT)

Copyright (c) 2016-2018 Oleg Linkin <maledictusdemagog@gmail.com>

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

#include <QRegularExpression>
#include <QTimer>

#include "src/enumsproxy.h"
#include "src/lj-rpc/ljxmlrpc.h"
#include "src/ljevent.h"
#include "src/ljeventproperties.h"
#include "src/models/ljfriendsmodel.h"
#include "src/models/friendssortfilterproxymodel.h"
#include "src/models/ljcommentsmodel.h"
#include "src/models/ljeventsmodel.h"
#include "src/models/ljfriendsgroupsmodel.h"
#include "src/models/ljmessagesmodel.h"
#include "src/models/messagessortfilterproxymodel.h"
#include "src/models/ljfriendsmodel.h"
#include "src/settings/accountsettings.h"
#include "src/settings/applicationsettings.h"
#include "src/utils.h"

namespace Mnemosy
{
MnemosyManager::MnemosyManager(QObject *parent)
: QObject(parent)
, m_IsBusy(false)
, m_IsLogged(false)
, m_FriendsPageModel(new LJEventsModel(this))
, m_CommentsModel(new LJCommentsModel(this))
, m_GroupsModel(new LJFriendGroupsModel(this))
, m_MyJournalModel(new LJEventsModel(this))
, m_UserJournalModel(new LJEventsModel(this))
, m_LJFriendsModel(new LJFriendsModel(this))
, m_FriendsProxyModel(new FriendsSortFilterProxyModel(this))
, m_MessagesModel(new LJMessagesModel(this))
, m_MessagesProxyModel(new MessagesSortFilterProxyModel(this))
, m_NotificationsModel(new LJMessagesModel(this))
, m_LJXmlRPC(new LJXmlRPC())
{
    qRegisterMetaType<QMap<QString, QPair<quint64, QUrl>>>("mapOfPairs_t");

    MakeConnections();

    m_FriendsProxyModel->setSourceModel(m_LJFriendsModel);
    m_MessagesProxyModel->setSourceModel(m_MessagesModel);

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

QVariantMap MnemosyManager::GetProfile() const
{
    QVariantMap map;
    map["userId"] = m_Profile.GetUserID();
    map["avatarUrl"] = m_Profile.GetDefaultPicUrl();
    map["fullName"] = m_Profile.GetFullName();
    map["userName"] = m_Profile.GetUserName();
    map["birthday"] = m_Profile.GetBirthday();
    map["avatars"] = m_Profile.GetAvatarsList();
    return map;
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

LJEventsModel*MnemosyManager::GetMyJournalModel() const
{
    return m_MyJournalModel;
}

LJEventsModel*MnemosyManager::GetUserJournalModel() const
{
    return m_UserJournalModel;
}

FriendsSortFilterProxyModel*MnemosyManager::GetFriendsModel() const
{
    return m_FriendsProxyModel;
}

MessagesSortFilterProxyModel *MnemosyManager::GetMessagesModel() const
{
    return m_MessagesProxyModel;
}

LJMessagesModel *MnemosyManager::GetNotificationsModel() const
{
    return m_NotificationsModel;
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
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::requestFinished,
            this,
            [=]()
            {
                SetBusy(false);
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::error,
            this,
            [=](const QString& msg, int code, ErrorType type)
            {
                SetBusy(false);
                const QString errorMessage = (type == ETLiveJournal ?
                        (tr("LiveJournal error: %1").arg(GetLocalizedErrorMessage(code))) :
                        msg);
                emit error(errorMessage, type);
                if (code == 206) // ivalid user name
                {
                    emit invalidUserName();
                }
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::logged,
            this,
            [=](bool validated, const QString& login, const QString& password)
            {
                SetLogged(validated, login, password);
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::gotUserProfile,
            this,
            [=](const UserProfile& profile)
            {
                SetProfile(profile);
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::gotFriendsPage,
            this,
            [=](const LJEvents_t& events)
            {
                LJEvents_t newEvents = events;
                for (auto& event : newEvents)
                {
                    Utils::TryToFillEventFields(event);
                    bool hasArg = false;
                    QString ev = event.GetEvent();
                    Utils::SetImagesWidth(ev, hasArg);
                    PrepareSdelanoUNas(ev);
                    Utils::RemoveStyleTag(ev);
                    event.SetHasArg(hasArg);
                    Utils::FixUntaggedUrls(ev);
                    event.SetEvent(ev);
                    m_UserName2UserIdAndPicUrl[event.GetPosterName()] =
                            qMakePair(event.GetPosterID(), event.GetPosterPicUrl());
                    UpdateFriends();
                }
                m_FriendsPageModel->AddEvents(newEvents);
                emit friendsPageModelChanged();
           });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::gotEvent,
            this,
            [=](const LJEvent& event, ModelType mt)
            {
                LJEvent newEvent = event;
                QString ev = newEvent.GetFullEvent();
                Utils::RemoveStyleTag(ev);
                PrepareSdelanoUNas(ev);
                bool hasArg = false;
                Utils::SetImagesWidth(ev, hasArg);
                newEvent.SetFullHasArg(hasArg);
                Utils::FixUntaggedUrls(ev);
                newEvent.SetFullEvent(ev);
                switch (mt)
                {
                case MTFeed:
                    m_FriendsPageModel->MergeEvent(newEvent);
                    break;
                case MTMyBlog:
                    m_MyJournalModel->MergeEvent(newEvent);
                    break;
                case MTUserBlog:
                    m_UserJournalModel->MergeEvent(newEvent);
                    break;
                case MTUnknown:
                default:
                    break;
                };
                emit gotEvent(newEvent.ToMap());
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::gotEvents,
            this,
            [=](const LJEvents_t& events, ModelType mt)
            {
                LJEvents_t newEvents = events;
                for (auto& event : newEvents)
                {
                    bool hasArg = false;
                    Utils::TryToFillEventFields(event);
                    QString ev = event.GetEvent();
                    Utils::SetImagesWidth(ev, hasArg);
                    PrepareSdelanoUNas(ev);
                    Utils::RemoveStyleTag(ev);
                    Utils::MoveFirstImageToTheTop(ev);

                    event.SetHasArg(hasArg);
                    event.SetEvent(ev);

                    auto picKeyword = event.GetProperties().GetPostAvatar();
                    switch (mt)
                    {
                    case MTMyBlog:
                    {
                        if (event.GetPosterPicUrl().isEmpty() &&
                                picKeyword.startsWith("pic#"))
                        {
                            picKeyword = picKeyword.mid(4);
                            const QString avatar = QString("http://l-userpic.livejournal.com/%1/%2")
                                .arg(picKeyword)
                                .arg(m_Profile.GetUserID());
                            event.SetPosterPicUrl(QUrl(avatar));
                        }
                        else if (event.GetPosterPicUrl().isEmpty())
                        {
                            event.SetPosterPicUrl(m_Profile.GetDefaultPicUrl());
                        }
                        break;
                    }
                    case MTUserBlog:
                    {
                    }
                    default:
                        break;
                    }
                }
                switch (mt)
                {
                case MTMyBlog:
                {
                    m_MyJournalModel->AddEvents(newEvents);
                    emit myJournalModelChanged();
                }
                case MTUserBlog:
                    m_UserJournalModel->AddEvents(newEvents);
                    emit userJournalModelChanged();
                break;
                case MTUnknown:
                default:
                    break;
                };
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::eventEdited,
            this,
            [=](quint64 itemId, quint64 dItemId)
            {
                if (m_EditedEvents.contains(itemId))
                {
                    LJEvent event = m_EditedEvents.take(itemId);
                    event.SetDItemID(dItemId);
                    m_MyJournalModel->UpdateEvent(event);
                    emit myJournalModelChanged();
                    emit notify(tr("Entry was edited"));
                }
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::eventDeleted,
            this,
            [=](quint64 itemId)
            {
                m_MyJournalModel->DeleteEvent(itemId);
                emit myJournalModelChanged();
                emit notify(tr("Entry was deleted"));
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::eventPosted,
            this,
            [=]()
            {
                emit notify(tr("Entry was added. Refresh to see it"));
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::gotComments,
            this,
            [=](const LJPostComments& postComments)
            {
                auto tempPostComments = postComments;
                for (auto& comment : tempPostComments.m_Comments)
                {
                    QString comm = comment.GetBody();
                    Utils::RemoveStyleTag(comm);
                    Utils::FixUntaggedUrls(comm);
                    comment.SetBody(comm);
                }
                m_CommentsModel->AddComments(postComments);
                emit commentsModelChanged();
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::commentAdded,
            this,
            [=]()
            {
                emit notify(tr("Comment was added. Refresh to see it"));
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::commentEdited,
            this,
            [=](const quint64 dTalkId)
            {
                if (m_EditedCommentCommands.contains(dTalkId))
                {
                    if (m_EditedCommentCommands.contains(dTalkId))
                    {
                        EditCommentCommand cmd = m_EditedCommentCommands.take(dTalkId);
                        m_CommentsModel->EditComment(dTalkId, cmd.m_Subject, cmd.m_Body);
                        emit commentEdited(dTalkId, cmd.m_Subject, cmd.m_Body);
                    }
                }
                emit notify(tr("Comment was edited"));
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::commentsDeleted,
            this,
            [=](const QList<quint64>& deletedComments)
            {
                QString authorName;
                if (deletedComments.size() &&
                        m_DeletedComment2AuthorName.contains(deletedComments.first()))
                {
                    authorName = m_DeletedComment2AuthorName[deletedComments.first()];
                    m_DeletedComment2AuthorName.remove(deletedComments.first());
                }
                m_CommentsModel->MarkCommentsAsDeleted(deletedComments, authorName);
                QVariantList result;
                std::copy(deletedComments.begin(), deletedComments.end(),
                        std::back_inserter(result));
                emit commentsDeleted(result, authorName);
                emit notify(tr("Comment(s) was deleted"));
            });

    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::gotFriendGroups,
            this,
            [=](const LJFriendGroups_t& groups)
            {
                m_Profile.SetFriendGroups(groups.toSet());
                m_GroupsModel->SetItems(groups);
                emit groupsModelChanged();
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::groupAdded,
            this,
            [=]()
            {
                if (!m_AddFriendGroupsRequestQueue.empty())
                {
                    LJFriendGroup group = m_AddFriendGroupsRequestQueue.dequeue();
                    m_GroupsModel->AddGroup(group);
                    emit notify(tr("Friend group was added"));
                }
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::groupDeleted,
            this,
            [=]()
            {
                if (!m_DeleteFriendGroupsRequestQueue.empty())
                {
                    quint64 groupId = m_DeleteFriendGroupsRequestQueue.dequeue();
                    m_GroupsModel->RemoveGroup(groupId);
                    emit notify(tr("Friend group was removed"));
                }
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::gotFriends,
            this,
            [=](const LJFriends_t& friends)
            {
                m_LJFriendsModel->SetItems(friends);
                UpdateFriends();
                emit friendsModelChanged();
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::friendAdded,
            this,
            [=](const LJFriend& fr)
            {
                LJFriend friendEntry = fr;
                friendEntry.SetMyFriend(true);
                m_LJFriendsModel->AddFriend(friendEntry);
                emit friendsModelChanged();
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::friendEdited,
            this,
            [=]()
            {
                if (!m_EditFriendRequestQueue.empty())
                {
                    auto pair = m_EditFriendRequestQueue.dequeue();
                    m_LJFriendsModel->EditFriend(pair.first, pair.second);
                    emit notify(tr("Friend was edited successfully"));
                }
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::friendDeleted,
            this,
            [=]()
            {
                if (!m_DeleteFriendRequestQueue.empty())
                {
                    auto name = m_DeleteFriendRequestQueue.dequeue();
                    m_LJFriendsModel->DeleteFriend(name);
                    emit notify(tr("Friend was removed from your friendlist"));
                }
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::gotMessages,
            this,
            [=](const LJMessages_t& messages)
            {
                m_MessagesModel->AddMessages(messages);
                ApplicationSettings::Instance()->setValue("lastMessagesSync",
                        QDateTime::currentDateTime().toTime_t());
                emit messagesModelChanged();
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::gotNotifications,
            this,
            [=](const LJMessages_t& messages)
            {
                m_NotificationsModel->AddMessages(messages);
                ApplicationSettings::Instance()->setValue("lastNotificationsSync",
                        QDateTime::currentDateTime().toTime_t());
                emit notificationsModelChanged();
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::gotReadMessages,
            this,
            [=](const QList<quint64>& qids)
            {
                m_MessagesModel->MarkAsRead(qids);
                emit messagesModelChanged();
                m_NotificationsModel->MarkAsRead(qids);
                emit notificationsModelChanged();
            });
    connect(m_LJXmlRPC.get(),
            &LJXmlRPC::messageSent,
            this,
            [=]()
            {
                emit notify(tr("Message was sent. Refresh to see it"));
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

void MnemosyManager::SetProfile(const UserProfile& profile)
{
    m_Profile.UpdateProfile(profile);
    m_GroupsModel->SetItems(m_Profile.GetFriendGroups().toList());
    emit profileChanged();
}

void MnemosyManager::SetLogged(bool logged, const QString& login,
        const QString& password)
{
    if (!logged)
    {
        AccountSettings::Instance(this)->setValue("login", QVariant());
        AccountSettings::Instance(this)->setValue("password", QVariant());
        ClearCache();
        ClearModels();
    }
    else
    {
        if (AccountSettings::Instance(this)->value("login").toString() != login)
        {
            ClearModels();
        }
        AccountSettings::Instance(this)->setValue("login", login);
        AccountSettings::Instance(this)->setValue("password", password);
    }

    SetLogged(logged);
}

void MnemosyManager::ClearCache()
{
    auto dataDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QSettings settings(dataDir + "/mnemosy_cache", QSettings::IniFormat);
    for (const auto& key : settings.allKeys())
    {
        settings.remove(key);
    }
    settings.sync();
}

void MnemosyManager::CacheEvents()
{
    SaveItems("my_blog", m_MyJournalModel->GetItems());
    SaveItems("friends", m_LJFriendsModel->GetItems());
    SaveItems("notifications", m_NotificationsModel->GetItems());
    SaveItems("messages", m_MessagesModel->GetItems());
    QList<UserProfile> profiles;
    profiles << m_Profile;
    SaveItems("profile", profiles);

    auto path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
            "/mnemosy_cache";
    QSettings settings(path, QSettings::IniFormat);
    settings.setValue("searched_users", QStringList(m_SearchedUsers.mid(0, 10)));
}

void MnemosyManager::LoadCachedEvents()
{
    LJEvents_t myEvents;
    LoadItems("my_blog", myEvents);
    m_MyJournalModel->SetItems(myEvents);

    LJFriends_t friends;
    LoadItems("friends", friends);
    m_LJFriendsModel->SetItems(friends);

    LJMessages_t notifications;
    LoadItems("notifications", notifications);
    m_NotificationsModel->SetItems(notifications);

    LJMessages_t messages;
    LoadItems("messages", messages);
    m_MessagesModel->SetItems(messages);

    QList<UserProfile> profiles;
    LoadItems("profile", profiles);
    SetProfile(profiles.value(0));

    auto path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
            "/mnemosy_cache";
    QSettings settings(path, QSettings::IniFormat);
    m_SearchedUsers = settings.value("searched_users").toStringList();
}

bool MnemosyManager::isMyFriend(const QString& name) const
{
    const auto& friends = m_LJFriendsModel->GetItems();
    auto it = std::find_if(friends.begin(), friends.end(),
            [name](decltype (friends.front()) fr)
            { return fr.GetUserName() == name; });
    return it == friends.end() ? false : (*it).GetMyFriend();
}

QStringList MnemosyManager::getAvailablePostTargets()
{
    QStringList result;
    result << m_Profile.GetUserName();
    result << m_Profile.GetCommunities();
    return result;
}

QStringList MnemosyManager::getSearchedUsers() const
{
    return m_SearchedUsers;
}

void MnemosyManager::SavePosterPicUrls()
{
    auto path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
            "/mnemosy_cache";
    QSettings settings(path, QSettings::IniFormat);
    settings.setValue("posterIdAndPicUrl",
           QVariant::fromValue(m_UserName2UserIdAndPicUrl));
}

void MnemosyManager::LoadPosterPicUrls()
{
    auto path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
            "/mnemosy_cache";
    QSettings settings(path, QSettings::IniFormat);
    QVariant map = settings.value("posterIdAndPicUrl");
    m_UserName2UserIdAndPicUrl = map.value<mapOfPairs_t>();
}


void MnemosyManager::ClearModels()
{
    if (m_FriendsPageModel)
    {
        m_FriendsPageModel->Clear();
    }

    if (m_CommentsModel)
    {
        m_CommentsModel->Clear();
    }

    if (m_GroupsModel)
    {
        m_GroupsModel->Clear();
    }

    if (m_LJFriendsModel)
    {
        m_LJFriendsModel->Clear();
    }

    if (m_NotificationsModel)
    {
        m_NotificationsModel->Clear();
    }

    if (m_MessagesModel)
    {
        m_MessagesModel->Clear();
    }
}

void MnemosyManager::UpdateFriends()
{
    if (!m_LJFriendsModel->GetCount())
    {
        return;
    }
    auto it = m_UserName2UserIdAndPicUrl.begin();
    for (; it != m_UserName2UserIdAndPicUrl.end(); ++it)
    {
        m_LJFriendsModel->SetFriendAvatar(it.key(), it.value().second);
    }
}

void MnemosyManager::abortRequest()
{
    m_LJXmlRPC->AbortRequest();
}

void MnemosyManager::login(const QString& login, const QString& password)
{
    if (login.isEmpty() || password.isEmpty())
    {
        return;
    }
    SetBusy(true);
    m_LJXmlRPC->Login(login, password);
}

void MnemosyManager::logout()
{
    ApplicationSettings::Instance(this)->remove("eventTarget");
    ApplicationSettings::Instance(this)->remove("eventCommentsEnabled");
    ApplicationSettings::Instance(this)->remove("eventNotifyByEmail");
    ApplicationSettings::Instance(this)->remove("eventAdult");
    ApplicationSettings::Instance(this)->remove("eventScreening");
    ApplicationSettings::Instance(this)->remove("eventSecurity");
    ApplicationSettings::Instance(this)->remove("eventGroupMask");
    ApplicationSettings::Instance(this)->remove("friendsSort");
    ApplicationSettings::Instance(this)->remove("friendsPageFilter");
    ApplicationSettings::Instance(this)->remove("messageDirection");
    ApplicationSettings::Instance(this)->remove("lastMessagesSync");
    ApplicationSettings::Instance(this)->remove("lastNotificationsSync");

    SetLogged(false, "", "");
}

void MnemosyManager::getFriendsPage()
{
    SetBusy(true);
    m_FriendsPageModel->Clear();
    m_LJXmlRPC->GetFriendsPage(QDateTime::currentDateTime(),
            ApplicationSettings::Instance(this)->value("friendsPageFilter", 0)
                    .toInt());
}

void MnemosyManager::getNextFriendsPage()
{
    SetBusy(true);
    m_LJXmlRPC->GetFriendsPage(m_FriendsPageModel->GetLastItemLogTime(),
            ApplicationSettings::Instance(this)->value("friendsPageFilter", 0)
                   .toInt());
}

void MnemosyManager::getEvent(quint64 dItemId, const QString& journalName,
        int modelType)
{
    SetBusy(true);
    m_LJXmlRPC->GetEvent(dItemId, journalName, static_cast<ModelType>(modelType));
}

void MnemosyManager::addComment(const QString& journalName, quint64 parentTalkId,
        quint64 dItemId, const QString& subject, const QString& body)
{
    SetBusy(true);
    m_LJXmlRPC->AddComment(journalName, parentTalkId, dItemId, subject, body);
}

void MnemosyManager::editComment(const QString& journalName, quint64 dTalkId,
    const QString& subject, const QString& body)
{
    SetBusy(true);
    m_EditedCommentCommands[dTalkId] = { dTalkId, subject, body };
    m_LJXmlRPC->EditComment(journalName, dTalkId, subject, body);
}

void MnemosyManager::deleteComment(const QString& journalName, quint64 dTalkId, int deleteMask,
        const QString& commentPoster)
{
    SetBusy(true);
    if (deleteMask & DCTAllComments)
    {
        m_DeletedComment2AuthorName.insert(dTalkId, commentPoster);
    }
    m_LJXmlRPC->DeleteComment(journalName, dTalkId, deleteMask);
}

void MnemosyManager::getComments(quint64 dItemId, const QString& journal,
        int page, quint64 dTalkId)
{
    SetBusy(true);
    if (dTalkId)
    {
        m_CommentsModel->Clear();
    }
    m_LJXmlRPC->GetComments(dItemId, journal, page, dTalkId);
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
    m_LJXmlRPC->GetFriendGroups();
}

void MnemosyManager::addFriendGroup(const QString& name, bool isPrivate)
{
    const int id = GetFreeGroupId(m_Profile.GetFriendGroups().toList());
    if (id == -1)
    {
        qWarning () << Q_FUNC_INFO
                << "You cannot add more groups: the limit of 30 groups is reached";
        emit error(tr("The limit of 30 groups is reached. Delete old before adding new one"),
                ETGeneral);
        return;
    }
    SetBusy(true);

    LJFriendGroup group;
    group.SetId(id);
    group.SetName(name);
    group.SetPublic(!isPrivate);
    group.SetRealId((1 << id) + 1);
    m_AddFriendGroupsRequestQueue.enqueue(group);

    m_LJXmlRPC->AddFriendGroup(name, isPrivate, id);
}

void MnemosyManager::deleteFriendGroup(quint64 groupId)
{
    SetBusy(true);
    m_DeleteFriendGroupsRequestQueue.enqueue(groupId);
    m_LJXmlRPC->DeleteFriendGroup(groupId);
}

void MnemosyManager::loadUserJournal(const QString& journalName, int modelType)
{
    SetBusy(true);
    if (static_cast<ModelType>(modelType) == MTMyBlog)
    {
        m_MyJournalModel->Clear();
    }
    else
    {
        m_UserJournalModel->Clear();
    }
    m_LJXmlRPC->LoadUserJournal(journalName, QDateTime::currentDateTime(),
            static_cast<ModelType>(modelType));
}

void MnemosyManager::loadNextUserJournalPage(const QString& journalName,
        int modelType)
{
    SetBusy(true);
    if (static_cast<ModelType>(modelType) == MTMyBlog)
    {
        m_LJXmlRPC->LoadUserJournal(journalName,
                m_MyJournalModel->GetLastItemLogTime(), MTMyBlog);
    }
    else
    {
        m_LJXmlRPC->LoadUserJournal(journalName,
                m_UserJournalModel->GetLastItemLogTime(), MTUserBlog);
    }
}

void MnemosyManager::getFriends()
{
    SetBusy(true);
    m_LJFriendsModel->Clear();
    m_LJXmlRPC->GetFriends();
}

void MnemosyManager::addFriend(const QString& name, uint groupMask)
{
    SetBusy(true);
    m_LJXmlRPC->AddFriend(name, groupMask);
}

void MnemosyManager::editFriend(const QString& name, uint groupMask)
{
    SetBusy(true);
    m_EditFriendRequestQueue.enqueue({ name, groupMask });
    m_LJXmlRPC->EditFriend(name, groupMask);
}

void MnemosyManager::deleteFriend(const QString& name)
{
    SetBusy(true);
    m_DeleteFriendRequestQueue.enqueue(name);
    m_LJXmlRPC->DeleteFriend(name);
}

void MnemosyManager::getMessages()
{
    SetBusy(true);
    m_LJXmlRPC->GetMessages(ApplicationSettings::Instance(this)->value("lastMessagesSync",
            QDateTime(QDateTime::currentDateTime().date().addDays(m_MonthAgo)).toTime_t())
                    .toLongLong());
}

void MnemosyManager::markMessageAsRead(const quint64 id)
{
    if (m_MessagesModel->IsUnread(id))
    {
        SetBusy(true);
        m_LJXmlRPC->MarkAsRead({ id });
    }
}

void MnemosyManager::markAllMessagesAsRead()
{
    SetBusy(true);
    m_LJXmlRPC->MarkAsRead(m_MessagesModel->GetUnread());
}

void MnemosyManager::getNotifications()
{
    SetBusy(true);
    m_LJXmlRPC->GetNotifications(ApplicationSettings::Instance(this)->value("lastNotificationsSync",
            QDateTime(QDateTime::currentDateTime().date().addDays(m_MonthAgo)).toTime_t())
                    .toLongLong());
}

void MnemosyManager::markNotificationAsRead(const quint64 id)
{
    SetBusy(true);
    m_LJXmlRPC->MarkAsRead({ id });
}

void MnemosyManager::markAllNotificationsAsRead()
{
    SetBusy(true);
    m_LJXmlRPC->MarkAsRead(m_NotificationsModel->GetUnread());
}

void MnemosyManager::sendMessage(const QString& to, const QString& subject, const QString& body,
        const qint64 parentMessageId)
{
    SetBusy(true);
    m_LJXmlRPC->SendMessage(to, subject, body, parentMessageId);
}

void MnemosyManager::postEvent(const QString& subject, const QString& body, const QString& tags,
        bool commentsEnabled, bool notifyByEmail, const QString& target, int adult, int screening, int security,
        uint groupMask, const QString& avatarId, const QString& location)
{
    SetBusy(true);
    LJEvent event;
    event.SetSubject(subject);
    event.SetEvent(body);
    event.SetAccess(static_cast<Access>(security));
    event.SetAllowMask(groupMask);
    event.SetJournalName(target);
    event.SetTags(tags.split(',', QString::SkipEmptyParts));
    event.SetPostDate(QDateTime::currentDateTime());
    LJEntryProperties props;
    props.SetAdultContent(static_cast<AdultContent>(adult));
    props.SetCommentsEnabled(commentsEnabled);
    props.SetCommentsManagement(static_cast<CommentsManagement>(screening));
    props.SetNotifyByEmail(notifyByEmail);
    props.SetPostAvatar(avatarId);
    props.SetCurrentLocation(location);
    event.SetProperties(props);
    m_LJXmlRPC->PostEvent(event);
}

void MnemosyManager::editEvent(quint64 itemId, const QString& subject, const QString& body,
        const QString& tags, bool commentsEnabled, bool notifyByEmail, const QString& target,
        int adult, int screening, int security, uint groupMask, const QString& avatarId,
        const QString& location)
{
    SetBusy(true);
    LJEvent event;
    event.SetItemID(itemId);
    event.SetSubject(subject);
    event.SetEvent(body);
    event.SetAccess(static_cast<Access>(security));
    event.SetAllowMask(groupMask);
    event.SetJournalName(target);
    event.SetTags(tags.split(',', QString::SkipEmptyParts));
    event.SetPostDate(QDateTime::currentDateTime());
    LJEntryProperties props;
    props.SetAdultContent(static_cast<AdultContent>(adult));
    props.SetCommentsEnabled(commentsEnabled);
    props.SetCommentsManagement(static_cast<CommentsManagement>(screening));
    props.SetNotifyByEmail(notifyByEmail);
    props.SetPostAvatar(avatarId);
    props.SetCurrentLocation(location);
    event.SetProperties(props);
    m_EditedEvents[itemId] = event;
    m_LJXmlRPC->EditEvent(event);
}

void MnemosyManager::deleteEvent(quint64 itemId, const QString& journal)
{
    SetBusy(true);
    m_LJXmlRPC->DeleteEvent(itemId, journal);
}

void MnemosyManager::showError(const QString& msg, int type)
{
    emit error(msg, type);
}

void MnemosyManager::saveSearchedUser(const QString& userName)
{
    m_SearchedUsers.insert(0, userName);
}
} // namespace Mnemosy
