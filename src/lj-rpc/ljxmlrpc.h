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

#include <functional>
#include <memory>

#include <QColor>
#include <QDomDocument>
#include <QObject>
#include <QMap>
#include <QPair>
#include <QPointer>
#include <QQueue>

#include "src/enumsproxy.h"
#include "src/ljcomment.h"
#include "src/ljevent.h"
#include "src/ljfriend.h"
#include "src/ljmessage.h"
#include "src/userprofile.h"

class QNetworkAccessManager;
class QNetworkReply;

namespace Mnemosy
{
enum class SelectType
{
    One,
    Day,
    LastN,
    SyncItems,
    Multiple,
    Before
};

class LJXmlRPC : public QObject
{
    Q_OBJECT

    struct GetEventsInfo
    {
        QString m_Key;
        QString m_Type;
        QString m_Value;
    };

    QNetworkAccessManager *m_NAM;
    QQueue<std::function<void (const QString&)>> m_ApiCallQueue;
    QPointer<QNetworkReply> m_CurrentReply;
public:
    static const int ItemShow = 20;
    static const int TrimWidgets = 300; // derived empirically
    static const int WidgetsImgLength = 150; //derived empirically
    static const int CommentsPageSize = 25; //derived empirically

    explicit LJXmlRPC(QObject *parent = 0);

    void AbortRequest();

    void Login(const QString& login, const QString& password);

    void GetFriendsPage(const QDateTime& before, int groupMask = 0);

    void GetEvent(quint64 dItemId, const QString& journalName, ModelType mt);

    void AddComment(const QString& journalName, quint64 parentTalkId,
            quint64 dItemId, const QString& subject, const QString& body, const QString& avatarId);
    void EditComment(const QString& journalName, quint64 dTalkId,
            const QString& subject, const QString& body);
    void DeleteComment(const QString& journalName, quint64 dTalkId, int deleteMask);
    void GetComments(quint64 dItemId, const QString& journal, int page = 1,
            quint64 dTalkId = 0);
    void GetUpdatedComment(quint64 dItemId, const QString& journal, quint64 dTalkId);
    void UpdateComment(quint64 dItemId, const QString& journalName, quint64 dTalkId,
            LJComment::Privilege action);

    void GetFriendGroups();
    void AddFriendGroup(const QString& name, bool isPrivate, int id);
    void DeleteFriendGroup(quint64 groupId);

    void LoadUserJournal(const QString& journalName, const QDateTime& before,
            ModelType mt);

    void GetFriends();
    void AddFriend(const QString& name, uint groupMask);
    void EditFriend(const QString& name, uint groupMask);
    void DeleteFriend(const QString& name);

    void GetMessages(quint64 lastSyncTime, const LJMessages_t& msgs = LJMessages_t());
    void GetNotifications(quint64 lastSyncTime, const LJMessages_t& msgs = LJMessages_t());
    void MarkAsRead(const QList<quint64>& qids);
    void SendMessage(const QString& to, const QString& subject, const QString& body,
            const qint64 parentMessageId = -1);

    void PostEvent(const LJEvent& event);
    void EditEvent(const LJEvent& event);
    void DeleteEvent(const quint64 itemId, const QString& journal);
private:
    std::shared_ptr<void> MakeRunnerGuard();

    QDomDocument PreparsingReply(QObject *sender, bool& ok);
    QDomDocument ParseDocument(const QByteArray& data, bool& ok);
    QPair<int, QString> CheckOnLJErrors(const QDomDocument& doc);
    QDomDocument GenerateGetEventsRequest(const QList<GetEventsInfo>& infos,
            const QString& journalName, SelectType st, const QString& challenge);
    QDomDocument GenerateEditFriendsRequest(const QString& name, uint groupMask,
            const QString& challenge);
    QDomDocument GenerateGetInboxRequest(const quint64 lastSync,
            const QList<int>& types, const QString& challenge);

    void GetChallenge();

    void Login(const QString& login, const QString& password,
            const QString& challenge);

    void GetFriendsPage(const QDateTime& before, int groupMask,
            const QString& challenge);

    void GetEvents(const QList<GetEventsInfo>& info, const QString& journalName,
            SelectType st, ModelType mt, const QString& challenge);

    void AddComment(const QString& journalName, quint64 parentTalkId,
            quint64 dItemId, const QString& subject, const QString& body, const QString& avatarId,
            const QString& challenge);
    void EditComment(const QString& journalName, quint64 dTalkId,
            const QString& subject, const QString& body,
            const QString& challenge);
    void DeleteComment(const QString& journalName, quint64 dTalkId, int deleteMask,
            const QString& challenge);
    void GetComments(quint64 dItemId, const QString& journal, int page,
            quint64 dTalkId, const QString& challenge);
    void GetUpdatedComment(quint64 dItemId, const QString& journal, quint64 dTalkId,
            const QString& challenge);
    void UpdateComment(quint64 dItemId, const QString& journalName, quint64 dTalkId,
            LJComment::Privilege action, const QString& challenge);

    void GetFriendGroups(const QString& challenge);
    void AddFriendGroup(const QString& name, bool isPrivate, int id,
            const QString& challenge);
    void DeleteFriendGroup(quint64 groupId, const QString& challenge);

    void LoadUserJournal(const QString& journalName, const QDateTime& before,
            ModelType mt, const QString& challenge);

    void GetFriends(const QString& challenge);
    void AddFriend(const QString& name, uint groupMask,
            const QString& challenge);
    void EditFriend(const QString& name, uint groupMask,
            const QString& challenge);
    void DeleteFriend(const QString& name, const QString& challenge);

    void GetMessages(quint64 lastSyncTime, const QString& challenge,
            const LJMessages_t& msgs = LJMessages_t());
    void GetNotifications(quint64 lastSyncTime, const QString& challenge,
            const LJMessages_t& msgs = LJMessages_t());
    void MarkAsRead(const QList<quint64>& qids, const QString& challenge);
    void SendMessage(const QString& to, const QString& subject, const QString& body,
            const qint64 parentMessageId, const QString& challenge);

    void PostEvent(const LJEvent& event, const QString& challenge);
    void EditEvent(const LJEvent& event, const QString& challenge);
    void DeleteEvent(const quint64 itemId, const QString& journal, const QString& challenge);

private slots:
    void handleGetChallenge();

    void handleLogin(const QString& login, const QString& password);

    void handleGetFriendsPage();

    void handleGetEvents(const ModelType mt);

    void handleAddComment();
    void handleEditComment();
    void handleDeleteComment();
    void handleGetComments();
    void handleGetUpdatedComment();
    void handleUpdateComment(quint64 dItemId, const QString& journalName);

    void handleGetFriendGroups();
    void handleAddFriendGroup();
    void handleDeleteFriendGroup();

    void handleLoadUserJournal(const ModelType mt);

    void handleGetFriends();
    void handleAddFriends();
    void handleEditFriends();
    void handleDeleteFriends();

    void handleGetMessages(const LJMessages_t& msgs);
    void handleGetNotifications(const LJMessages_t& msgs);
    void handleMarkAsRead();
    void handleSendMessage();

    void handlePostEvent();
    void handleEditEvent();
    void handleDeleteEvent();

signals:
    void requestFinished();

    void logged(bool result, const QString& login, const QString& password);

    void gotUserProfile(const UserProfile& profile);

    void gotFriendsPage(const LJEvents_t& events);

    void gotEvent(const LJEvent& event, ModelType mt);
    void gotEvents(const LJEvents_t& events, ModelType mt);
    void eventDeleted(quint64 itemId);
    void eventEdited(quint64 itemId, quint64 dItemId);
    void eventPosted();

    void gotComments(const LJPostComments& postComments);
    void commentAdded();
    void commentEdited(const quint64 dTalkId);
    void commentsDeleted(const QList<quint64>& commentsIds);
    void commentsUpdated(const LJPostComments& postComments = LJPostComments());

    void gotFriendGroups(const LJFriendGroups_t& groups);
    void groupAdded();
    void groupDeleted();

    void gotFriends(const LJFriends_t& friends);
    void friendAdded(const LJFriend& fr);
    void friendEdited();
    void friendDeleted();

    void gotMessages(const LJMessages_t& messages);
    void gotNotifications(const LJMessages_t& messages);
    void gotReadMessages(const QList<quint64>& ids);
    void messageSent();

    void error(const QString& msg, int code = 0, ErrorType type = ETGeneral);
};
} // namespace Mnemosy
