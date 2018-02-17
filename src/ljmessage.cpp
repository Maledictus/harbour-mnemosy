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

#include "ljmessage.h"
#include <QDataStream>
#include <QRegularExpression>
#include <QtDebug>

namespace Mnemosy
{
LJMessage::LJMessage()
: m_QId(0)
, m_PosterId(0)
, m_EntryDItemId(0)
, m_DTalkID(0)
, m_ToId(0)
, m_MessageId(0)
, m_ParentID(0)
, m_FromId(0)
, m_State(SNoState)
, m_Type(MTNoType)
, m_Action(ANoAction)
, m_Direction(DUnknown)
{
}

quint64 LJMessage::GetQId() const
{
    return m_QId;
}

void LJMessage::SetQId(quint64 qid)
{
    m_QId = qid;
}

QDateTime LJMessage::GetDate() const
{
    return m_Date;
}

void LJMessage::SetDate(const QDateTime& dt)
{
    m_Date = dt;
}

QString LJMessage::GetSubject() const
{
    return m_Subject;
}

void LJMessage::SetSubject(const QString& subject)
{
    m_Subject = subject;
}

quint64 LJMessage::GetPosterId() const
{
    return m_PosterId;
}

void LJMessage::SetPosterId(quint64 posterId)
{
    m_PosterId = posterId;
}

quint64 LJMessage::GetEntryDItemId() const
{
    return m_EntryDItemId;
}

void LJMessage::SetEntryDItemId(const quint64 id)
{
    m_EntryDItemId = id;
}

QUrl LJMessage::GetPosterPicUrl() const
{
    return m_PosterPicUrl;
}

void LJMessage::SetPosterPicUrl(const QUrl& url)
{
    m_PosterPicUrl = url;
}

QUrl LJMessage::GetCommentUrl() const
{
    return m_CommentUrl;
}

void LJMessage::SetCommentUrl(const QUrl& url)
{
    m_CommentUrl = url;
}

QString LJMessage::GetEntrySubject() const
{
    return m_EntrySubject;
}

void LJMessage::SetEntrySubject(const QString& subject)
{
    m_EntrySubject = subject;
}

QString LJMessage::GetPoster() const
{
    return m_Poster;
}

void LJMessage::SetPoster(const QString& poster)
{
    m_Poster = poster;
}

QString LJMessage::GetBody() const
{
    return m_Body;
}

void LJMessage::SetBody(const QString& body)
{
    m_Body = body;
}

quint64 LJMessage::GetDTalkID() const
{
    return m_DTalkID;
}

void LJMessage::SetDTalkID(quint64 id)
{
    m_DTalkID = id;
}

QString LJMessage::GetTo() const
{
    return m_To;
}

void LJMessage::SetTo(const QString& to)
{
    m_To = to;
}

quint64 LJMessage::GetToId() const
{
    return m_ToId;
}

void LJMessage::SetToId(quint64 id)
{
    m_ToId = id;
}

quint64 LJMessage::GetMessageId() const
{
    return m_MessageId;
}

void LJMessage::SetMessageId(quint64 id)
{
    m_MessageId = id;
}

quint64 LJMessage::GetParentID() const
{
    return m_ParentID;
}

void LJMessage::SetParentID(quint64 id)
{
    m_ParentID = id;
}

quint64 LJMessage::GetFromId() const
{
    return m_FromId;
}

void LJMessage::SetFromId(quint64 id)
{
    m_FromId = id;
}

QString LJMessage::GetFrom() const
{
    return m_From;
}

void LJMessage::SetFrom(const QString& from)
{
    m_From = from;
}

LJMessage::State LJMessage::GetState() const
{
    return m_State;
}

void LJMessage::SetState(LJMessage::State st)
{
    m_State = st;
}

LJMessage::Type LJMessage::GetType() const
{
    return m_Type;
}

void LJMessage::SetType(Type t)
{
    m_Type = t;
}

LJMessage::Action LJMessage::GetAction() const
{
    return m_Action;
}

void LJMessage::SetAction(LJMessage::Action act)
{
    m_Action = act;
}

LJMessage::Direction LJMessage::GetDirection() const
{
    return m_Direction;
}

void LJMessage::SetDirection(LJMessage::Direction direction)
{
    m_Direction = direction;
}

QByteArray LJMessage::Serialize() const
{
    quint16 ver = 2;
    QByteArray result;
    {
        QDataStream ostr(&result, QIODevice::WriteOnly);
        ostr << ver
                << m_QId
                << m_Date
                << m_Subject
                << m_PosterId
                << m_EntryDItemId
                << m_PosterPicUrl
                << m_CommentUrl
                << m_EntrySubject
                << m_Poster
                << m_Body
                << m_DTalkID
                << m_To
                << m_ToId
                << m_MessageId
                << m_ParentID
                << m_FromId
                << m_From
                << static_cast<int>(m_State)
                << static_cast<int>(m_Type)
                << static_cast<int>(m_Action)
                << static_cast<int>(m_Direction)
                << m_JournalName;
    }

    return result;
}

namespace
{
LJMessage::State GetStateFromInt(int state)
{
    switch (state)
    {
    case 1:
        return LJMessage::SRead;
    case 2:
        return LJMessage::SUnread;
    case 0:
    default:
        return LJMessage::SNoState;
    }
}

LJMessage::Type GetTypeFromInt(int type)
{
    switch (type)
    {
    case 1:
        return LJMessage::MTBeFriended;
    case 2:
        return LJMessage::MTBirthday;
    case 3:
        return LJMessage::MTCommunityInvite;
    case 4:
        return LJMessage::MTCommunityJoinApprove;
    case 5:
        return LJMessage::MTCommunityJoinReject;
    case 6:
        return LJMessage::MTCommunityJoinRequest;
    case 7:
        return LJMessage::MTDefriended;
    case 8:
        return LJMessage::MTInvitedFriendJoins;
    case 9:
        return LJMessage::MTJournalNewComment;
    case 10:
        return LJMessage::MTJournalNewEntry;
    case 11:
        return LJMessage::MTNewUserpic;
    case 12:
        return LJMessage::MTNewVGift;
    case 13:
        return LJMessage::MTOfficialPost;
    case 14:
        return LJMessage::MTPermSale;
    case 15:
        return LJMessage::MTPollVote;
    case 16:
        return LJMessage::MTSupOfficialPost;
    case 17:
        return LJMessage::MTUserExpunged;
    case 18:
        return LJMessage::MTUserMessageRecvd;
    case 19:
        return LJMessage::MTUserMessageSent;
    case 20:
        return LJMessage::MTUserNewComment;
    case 21:
        return LJMessage::MTUserNewEntry;
    case 22:
        return LJMessage::MTCommentReply;
    case 0:
    default:
        return LJMessage::MTNoType;
    }
}

LJMessage::Action GetActionFromInt(int action)
{
    switch (action)
    {
    case 1:
        return LJMessage::ADeleted;
    case 2:
        return LJMessage::ACommentDeleted;
    case 3:
        return LJMessage::AEdited;
    case 4:
        return LJMessage::ANew;
    case 0:
    default:
        return LJMessage::ANoAction;
    }
}

LJMessage::Direction GetDirectionFromInt(int direction)
{
    switch (direction)
    {
    case 1:
        return LJMessage::DIn;
    case 2:
        return LJMessage::DOut;
    case 0:
    default:
        return LJMessage::DUnknown;
    }
}
}

bool LJMessage::Deserialize(const QByteArray& data, LJMessage& message)
{
    quint16 ver = 0;
    QDataStream in(data);
    in >> ver;

    if(ver > 2 || ver < 1)
    {
        qWarning() << Q_FUNC_INFO
                << "unknown version"
                << ver;
        return false;
    }

    int state = 0;
    int type = 0;
    int action = 0;
    int direction= 0;
    in >> message.m_QId
            >> message.m_Date
            >> message.m_Subject
            >> message.m_PosterId
            >> message.m_EntryDItemId
            >> message.m_PosterPicUrl
            >> message.m_CommentUrl
            >> message.m_EntrySubject
            >> message.m_Poster
            >> message.m_Body
            >> message.m_DTalkID
            >> message.m_To
            >> message.m_ToId
            >> message.m_MessageId
            >> message.m_ParentID
            >> message.m_FromId
            >> message.m_From
            >> state
            >> type
            >> action
            >> direction;
    message.SetState(GetStateFromInt(state));
    message.SetType(GetTypeFromInt(type));
    message.SetAction(GetActionFromInt(action));
    message.SetDirection(GetDirectionFromInt(direction));
    if (ver == 2)
    {
        in >> message.m_JournalName;
    }

    return true;
}

bool LJMessage::IsValid() const
{
    return m_QId > 0;
}

QString LJMessage::GetJournalName() const
{
    if (m_JournalName.isEmpty())
    {
        QRegularExpression exp("https?://(.+)\\.livejournal\\.com.+",
                QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = exp.match(m_CommentUrl.toString());
        return match.hasMatch() ? match.captured(1) : "";
    }

    return m_JournalName;
}

void LJMessage::SetJournalName(const QString& journal)
{
    m_JournalName = journal;
}

QVariantMap LJMessage::ToMap() const
{
    QVariantMap result;
    result["messageQId"] = m_QId;
    result["messagePostDate"] = m_Date;
    result["messageSubject"] = m_Subject;
    result["messagePosterId"] = m_PosterId;
    result["messageEntryDItemId"] = m_EntryDItemId;
    result["messageState"] = m_State;
    result["messagePosterAvatar"] = m_PosterPicUrl;
    result["messageCommentUrl"] = m_CommentUrl;
    result["messageEntrySubject"] = m_EntrySubject;
    result["messageAction"] = m_Action;
    result["messageType"] = m_Type;
    result["messagePosterName"] = m_Poster;
    result["messageBody"] = m_Body;
    result["messageDTalkId"] = m_DTalkID;
    result["messageTo"] = m_To;
    result["messageToId"] = m_ToId;
    result["messageFrom"] = m_From;
    result["messageFromId"] = m_FromId;
    result["messageId"] = m_MessageId;
    result["messageDirection"] = m_Direction;
    result["messageParentId"] = m_ParentID;
    result["messageJournalName"] = GetJournalName();
    return result;
}
} // namespace Mnemosy
