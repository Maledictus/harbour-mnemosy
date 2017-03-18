/*
The MIT License (MIT)

Copyright (c) 2016-2017 Oleg Linkin <maledictusdemagog@gmail.com>

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
#include <QQueue>
#include <QVariantMap>

#include "src/ljevent.h"
#include "src/ljfriendsgroup.h"

namespace Mnemosy
{
class LJXmlRPC;
class UserProfile;
class LJEventsModel;
class LJCommentsModel;
class LJFriendGroupsModel;
class LJFriendsModel;
class FriendsSortFilterProxyModel;

class MnemosyManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MnemosyManager)

    bool m_IsBusy;
    bool m_IsLogged;
    UserProfile *m_Profile;
    LJEventsModel *m_FriendsPageModel;
    LJCommentsModel *m_CommentsModel;
    LJFriendGroupsModel *m_GroupsModel;
    LJEventsModel *m_MyJournalModel;
    LJEventsModel *m_UserJournalModel;
    LJFriendsModel *m_FriendsModel;
    FriendsSortFilterProxyModel *m_FriendsProxyModel;

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
    QQueue<LJFriendGroup> m_AddFriendGroupsRequestQueue;
    QQueue<quint64> m_DeleteFriendGroupsRequestQueue;
    QQueue<QPair<QString, uint>> m_EditFriendRequestQueue;
    QQueue<QString> m_DeleteFriendRequestQueue;

    Q_PROPERTY(bool busy READ GetBusy NOTIFY busyChanged)
    Q_PROPERTY(bool logged READ GetLogged NOTIFY loggedChanged)
    Q_PROPERTY(UserProfile* profile READ GetProfile NOTIFY profileChanged)
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

    explicit MnemosyManager(QObject *parent = 0);
public:
    static MnemosyManager* Instance(QObject *parent = 0);
    bool GetBusy() const;
    bool GetLogged() const;
    UserProfile* GetProfile() const;
    LJEventsModel* GetFriendsPageModel() const;
    LJCommentsModel* GetCommentsModel() const;
    LJFriendGroupsModel* GetGroupsModel() const;
    LJEventsModel* GetMyJournalModel() const;
    LJEventsModel* GetUserJournalModel() const;
    FriendsSortFilterProxyModel* GetFriendsModel() const;

    void CacheEvents();
    void LoadCachedEvents();

    Q_INVOKABLE bool isMyFriend(const QString& name) const;
private:
    void MakeConnections();

    void SetBusy(const bool busy);
    void SetLogged(const bool logged);
    void SetProfile(UserProfile *profile);

    void SetLogged(bool logged, const QString& login, const QString& password);

    void ClearCache();
    void SaveItems(const QString& name, const LJEvents_t& events);
    void LoadItems(const QString& name, LJEventsModel *model);
    void SaveFriends();
    void LoadFriends();
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

    void showError(const QString& msg, int type);

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
