#pragma once

#include <QDateTime>
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

    QByteArray Serialize() const;
    static bool Deserialize(const QByteArray& data, LJMessage& message);
};

typedef QList<LJMessage> LJMessages_t;
} // namespace Mnemosy
