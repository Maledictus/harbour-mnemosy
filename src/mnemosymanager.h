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

#pragma once

#include <memory>

#include <QDir>
#include <QMap>
#include <QObject>
#include <QPointer>
#include <QQueue>
#include <QSettings>
#include <QStandardPaths>
#include <QVariantMap>
#include <QUrl>
#include <QtDebug>

#include "src/ljevent.h"
#include "src/ljfriendsgroup.h"
#include "src/userprofile.h"

namespace Mnemosy
{
class LJXmlRPC;
class LJEventsModel;
class LJCommentsModel;
class LJFriendGroupsModel;
class LJFriendsModel;
class FriendsSortFilterProxyModel;
class LJMessagesModel;
class MessagesSortFilterProxyModel;

class MnemosyManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MnemosyManager)

    static const int m_MonthAgo = -30;

    bool m_IsBusy;
    bool m_IsLogged;
    UserProfile m_Profile;
    LJEventsModel *m_FriendsPageModel;
    LJCommentsModel *m_CommentsModel;
    LJFriendGroupsModel *m_GroupsModel;
    LJEventsModel *m_MyJournalModel;
    LJEventsModel *m_UserJournalModel;
    LJFriendsModel *m_LJFriendsModel;
    FriendsSortFilterProxyModel *m_FriendsProxyModel;
    LJMessagesModel *m_MessagesModel;
    MessagesSortFilterProxyModel *m_MessagesProxyModel;
    LJMessagesModel *m_NotificationsModel;
    QStringList m_SearchedUsers;

    QMap<QString, QPair<quint64, QUrl>> m_UserName2UserIdAndPicUrl;

    std::shared_ptr<LJXmlRPC> m_LJXmlRPC;

    QMap<quint64, QString> m_DeletedComment2AuthorName;

    struct EditCommentCommand
    {
        quint64 m_DTalkId;
        QString m_Subject;
        QString m_Body;
    };
    QMap<quint64, EditCommentCommand> m_EditedCommentCommands;
    QMap<quint64, LJEvent> m_EditedEvents;
    QQueue<LJFriendGroup> m_AddFriendGroupsRequestQueue;
    QQueue<quint64> m_DeleteFriendGroupsRequestQueue;
    QQueue<QPair<QString, uint>> m_EditFriendRequestQueue;
    QQueue<QString> m_DeleteFriendRequestQueue;

    Q_PROPERTY(bool busy READ GetBusy NOTIFY busyChanged)
    Q_PROPERTY(bool logged READ GetLogged NOTIFY loggedChanged)
    Q_PROPERTY(QVariantMap userProfile READ GetProfile NOTIFY profileChanged)
    Q_PROPERTY(LJEventsModel* friendsPageModel READ GetFriendsPageModel
            NOTIFY friendsPageModelChanged)
    Q_PROPERTY(LJCommentsModel* commentsModel READ GetCommentsModel
            NOTIFY commentsModelChanged)
    Q_PROPERTY(LJFriendGroupsModel* groupsModel READ GetGroupsModel
            NOTIFY groupsModelChanged)
    Q_PROPERTY(LJEventsModel* myJournalModel READ GetMyJournalModel
            NOTIFY myJournalModelChanged)
    Q_PROPERTY(LJEventsModel* userJournalModel READ GetUserJournalModel
            NOTIFY userJournalModelChanged)
    Q_PROPERTY(FriendsSortFilterProxyModel* friendsModel READ GetFriendsModel
            NOTIFY friendsModelChanged)
    Q_PROPERTY(MessagesSortFilterProxyModel* messagesModel READ GetMessagesModel
            NOTIFY messagesModelChanged)
    Q_PROPERTY(LJMessagesModel* notificationsModel READ GetNotificationsModel
            NOTIFY notificationsModelChanged)

    explicit MnemosyManager(QObject *parent = 0);
public:
    static MnemosyManager* Instance(QObject *parent = 0);
    bool GetBusy() const;
    bool GetLogged() const;
    QVariantMap GetProfile() const;
    LJEventsModel* GetFriendsPageModel() const;
    LJCommentsModel* GetCommentsModel() const;
    LJFriendGroupsModel* GetGroupsModel() const;
    LJEventsModel* GetMyJournalModel() const;
    LJEventsModel* GetUserJournalModel() const;
    FriendsSortFilterProxyModel* GetFriendsModel() const;
    MessagesSortFilterProxyModel* GetMessagesModel() const;
    LJMessagesModel* GetNotificationsModel() const;

    void CacheEvents();
    void LoadCachedEvents();

    Q_INVOKABLE bool isMyFriend(const QString& name) const;
    Q_INVOKABLE QStringList getAvailablePostTargets();
    Q_INVOKABLE QStringList getSearchedUsers() const;
private:
    void MakeConnections();

    void SetBusy(const bool busy);
    void SetLogged(const bool logged);
    void SetProfile(const UserProfile& profile);

    void SetLogged(bool logged, const QString& login, const QString& password);

    void ClearCache();

    template<typename T>
    void SaveItems(const QString& name, const QList<T>& items)
    {
        qDebug() << Q_FUNC_INFO
                << name
                << "elements count:" << items.count();
        auto dataDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        QDir dir(dataDir);
        if (!dir.exists())
        {
            dir.mkpath(dataDir);
        }

        QSettings settings(dataDir + "/mnemosy_cache", QSettings::IniFormat);
        settings.beginWriteArray(name);
        for (int i = 0, size = items.size(); i < size; ++i)
        {
            settings.setArrayIndex(i);
            settings.setValue("SerializedData", items.at(i).Serialize());
        }
        settings.endArray();
        settings.sync();
    }

    template<typename T>
    void LoadItems(const QString& name, QList<T>& items)
    {
        auto path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
                "/mnemosy_cache";
        QSettings settings(path, QSettings::IniFormat);
        const int size = settings.beginReadArray(name);
        for (int i = 0; i < size; ++i)
        {
            settings.setArrayIndex(i);
            QByteArray data = settings.value("SerializedData").toByteArray();
            T item;
            if (!T::Deserialize(data, item) || !item.IsValid())
            {
                qWarning() << Q_FUNC_INFO
                        << "unserializable item"
                        << i;
                continue;
            }
            items << item;
        }
        settings.endArray();
    }

    void SavePosterPicUrls();
    void LoadPosterPicUrls();

    void ClearModels();

    void UpdateFriends();

public slots:
    void abortRequest();

    void login(const QString& login, const QString& password);
    void logout();

    void getFriendsPage();
    void getNextFriendsPage();

    void getEvent(quint64 dItemId, const QString& journalName, int modelType);

    void addComment(const QString& journalName, quint64 parentTalkId,
            quint64 dItemId, const QString& subject, const QString& body);
    void editComment(const QString& journalName, quint64 dTalkId,
            const QString& subject, const QString& body);
    void deleteComment(const QString& journalName, quint64 dTalkId, int deleteMask,
            const QString& commentPoster = QString());
    void getComments(quint64 dItemId, const QString& journal, int page = 1,
            quint64 dTalkId = 0);

    void getFriendGroups();
    void addFriendGroup(const QString& name, bool isPrivate);
    void deleteFriendGroup(quint64 groupId);

    void loadUserJournal(const QString& journalName, int modelType);
    void loadNextUserJournalPage(const QString& journalName, int modelType);

    void getFriends();
    void addFriend(const QString& name, uint groupMask);
    void editFriend(const QString& name, uint groupMask);
    void deleteFriend(const QString& name);

    void getMessages();
    void markMessageAsRead(const quint64 id);
    void markAllMessagesAsRead();
    void getNotifications();
    void markNotificationAsRead(const quint64 id);
    void markAllNotificationsAsRead();
    void sendMessage(const QString& to, const QString& subject, const QString& body,
            const qint64 parentMessageId = -1);

    void postEvent(const QString& subject, const QString& body, const QString& tags,
            bool commentsEnabled, bool notifyByEmail, const QString& target, int adult, int screening,
            int security, uint groupMask);
    void editEvent(quint64 itemId, const QString& subject, const QString& body, const QString& tags,
            bool commentsEnabled, bool notifyByEmail, const QString& target, int adult, int screening,
            int security, uint groupMask);
    void deleteEvent(quint64 itemId, const QString& journal);

    void showError(const QString& msg, int type);

    void saveSearchedUser(const QString& userName);

signals:
    void busyChanged();
    void loggedChanged();
    void profileChanged();
    void friendsPageModelChanged();
    void commentsModelChanged();
    void groupsModelChanged();
    void myJournalModelChanged();
    void userJournalModelChanged();
    void friendsModelChanged();
    void messagesModelChanged();
    void notificationsModelChanged();

    void gotEvent(const QVariantMap& newEvent);

    void error(const QString& msg, int type);
    void notify(const QString& msg);

    void invalidUserName();

    void commentsDeleted(const QVariantList& comments, const QString& posterName);
    void commentEdited(const quint64 dTalkId, const QString& subject, const QString& body);
};
} // namespace Mnemosy

typedef QMap<QString, QPair<quint64, QUrl>> mapOfPairs_t;
Q_DECLARE_METATYPE(mapOfPairs_t)
