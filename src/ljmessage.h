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

#include <QDateTime>
#include <QVariant>
#include <QUrl>

namespace Mnemosy
{
class LJMessage
{
    quint64 m_QId;

    QDateTime m_Date;
    QString m_Subject;
    quint64 m_PosterId;
    quint64 m_EntryDItemId;
    QUrl m_PosterPicUrl;
    QUrl m_CommentUrl;
    QString m_EntrySubject;
    QString m_Poster;
    QString m_Body;
    quint64 m_DTalkID;
    QString m_To;
    quint64 m_ToId;
    quint64 m_MessageId;
    quint64 m_ParentID;
    quint64 m_FromId;
    QString m_From;
    QString m_JournalName;
public:
    enum Type
    {
        MTNoType = 0,
        MTBeFriended,
        MTBirthday,
        MTCommunityInvite,
        MTCommunityJoinApprove,
        MTCommunityJoinReject,
        MTCommunityJoinRequest,
        MTDefriended,
        MTInvitedFriendJoins,
        MTJournalNewComment,
        MTJournalNewEntry,
        MTNewUserpic,
        MTNewVGift,
        MTOfficialPost,
        MTPermSale,
        MTPollVote,
        MTSupOfficialPost,
        MTUserExpunged,
        MTUserMessageRecvd,
        MTUserMessageSent,
        MTUserNewComment,
        MTUserNewEntry,
        MTCommentReply
    };

    enum Direction
    {
        DUnknown = 0,
        DIn,
        DOut
    };

    enum State
    {
        SNoState = 0,
        SRead,
        SUnread,
    };

    enum Action
    {
        ANoAction = 0,
        ADeleted,
        ACommentDeleted,
        AEdited,
        ANew
    };

private:
    State m_State;
    Type m_Type;
    Action m_Action;
    Direction m_Direction;

public:
    explicit LJMessage();

    quint64 GetQId() const;
    void SetQId(quint64 qid);
    QDateTime GetDate() const;
    void SetDate(const QDateTime& dt);
    QString GetSubject() const;
    void SetSubject(const QString& subject);
    quint64 GetPosterId() const;
    void SetPosterId(quint64 posterId);
    quint64 GetEntryDItemId() const;
    void SetEntryDItemId(const quint64 id);
    QUrl GetPosterPicUrl() const;
    void SetPosterPicUrl(const QUrl& url);
    QUrl GetCommentUrl() const;
    void SetCommentUrl(const QUrl& url);
    QString GetEntrySubject() const;
    void SetEntrySubject(const QString& subject);
    QString GetPoster() const;
    void SetPoster(const QString& poster);
    QString GetBody() const;
    void SetBody(const QString& body);
    quint64 GetDTalkID() const;
    void SetDTalkID(quint64 id);
    QString GetTo() const;
    void SetTo(const QString& to);
    quint64 GetToId() const;
    void SetToId(quint64 id);
    quint64 GetMessageId() const;
    void SetMessageId(quint64 id);
    quint64 GetParentID() const;
    void SetParentID(quint64 id);
    quint64 GetFromId() const;
    void SetFromId(quint64 id);
    QString GetFrom() const;
    void SetFrom(const QString& from);
    State GetState() const;
    void SetState(State st);
    Type GetType() const;
    void SetType(Type t);
    Action GetAction() const;
    void SetAction(Action act);
    Direction GetDirection() const;
    void SetDirection(Direction direction);
    QString GetJournalName() const;
    void SetJournalName(const QString& journal);

    QByteArray Serialize() const;
    static bool Deserialize(const QByteArray& data, LJMessage& message);

    bool IsValid() const;

    QVariantMap ToMap() const;
};

typedef QList<LJMessage> LJMessages_t;
} // namespace Mnemosy
