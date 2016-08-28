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

#include "ljevent.h"

#include <QtDebug>

namespace Mnemosy
{
LJEvent::LJEvent()
: m_UserID(0)
, m_UserPicID(0)
, m_PosterID(0)
, m_PosterJournalType(JTPersonal)
, m_JournalID(0)
, m_JournalType(JTPersonal)
, m_DItemID(0)
, m_Access(APublic)
, m_ReplyCount(0)
, m_ItemID(0)
, m_CanComment(true)
, m_AllowMask(0)
, m_HasArg(false)
, m_FullHasArg(false)
, m_Anum(0)
{
}

bool LJEvent::IsValid() const
{
    return m_ItemID || m_DItemID;
}

quint64 LJEvent::GetUserID() const
{
    return m_UserID;
}

void LJEvent::SetUserID(quint64 id)
{
    m_UserID = id;
}

quint64 LJEvent::GetUserPicID() const
{
    return m_UserPicID;
}

void LJEvent::SetUserPicID(quint64 id)
{
    m_UserPicID = id;
}

quint64 LJEvent::GetItemID() const
{
    return m_ItemID;
}

void LJEvent::SetItemID(quint64 itemId)
{
    m_ItemID = itemId;
}

quint64 LJEvent::GetDItemID() const
{
    return m_DItemID;
}

void LJEvent::SetDItemID(quint64 ditemId)
{
    m_DItemID = ditemId;
}

QString LJEvent::GetEvent() const
{
    return m_Event;
}

void LJEvent::SetEvent(const QString& event)
{
    m_Event = event;
}

QString LJEvent::GetSubject() const
{
    return m_Subject;
}

void LJEvent::SetSubject(const QString& subject)
{
    m_Subject = subject;
}

QDateTime LJEvent::GetPostDate() const
{
    return m_PostDate;
}

void LJEvent::SetPostDate(const QDateTime& date)
{
    m_PostDate = date;
}

QString LJEvent::GetJournalName() const
{
    return m_JournalName;
}

void LJEvent::SetJournalName(const QString& journalName)
{
    m_JournalName = journalName;
}

QUrl LJEvent::GetJournalUrl() const
{
    return m_JournalUrl;
}

void LJEvent::SetJournalUrl(const QUrl& url)
{
    m_JournalUrl = url;
}

QStringList LJEvent::GetTags() const
{
    return m_Tags;
}

void LJEvent::SetTags(const QStringList& tags)
{
    m_Tags = tags;
}

LJEntryProperties LJEvent::GetProperties() const
{
    return m_Properties;
}

void LJEvent::SetProperties(const LJEntryProperties& properties)
{
    m_Properties = properties;
}

JournalType LJEvent::GetJournalType() const
{
    return m_JournalType;
}

void LJEvent::SetJournalType(JournalType journalType)
{
    m_JournalType = journalType;
}

QByteArray LJEvent::Serialize() const
{
    quint16 ver = 2;
    QByteArray result;
    {
        QDataStream ostr(&result, QIODevice::WriteOnly);
        ostr << ver
                << m_UserID
                << m_UserPicID
                << m_PosterID
                << m_PosterUrl
                << m_PosterPicUrl
                << m_PosterName
                << m_PosterJournalType
                << m_JournalID
                << m_JournalType
                << m_JournalName
                << m_JournalUrl
                << m_DItemID
                << m_Subject
                << m_Event
                << m_PostDate
                << m_Tags
                << m_Access
                << m_Properties.Serialize()
                << m_ReplyCount
                << m_FullEvent
                << m_ItemID
                << m_CanComment
                << m_Url
                << m_HasArg;

        if (ver == 2)
        {
            ostr << m_FullHasArg;
            ostr << m_LogTime;
        }
    }


    return result;
}

LJEvent LJEvent::Deserialize(const QByteArray& data)
{
    quint16 ver = 0;
    QDataStream in(data);
    in >> ver;

    LJEvent result;
    if(ver > 3)
    {
        qWarning() << Q_FUNC_INFO
                << "unknown version"
                << ver;
        return result;
    }

    int journalType = 0;
    int posterJournalType = 0;
    int access = 0;
    QByteArray props;
    in >> result.m_UserID
            >> result.m_UserPicID
            >> result.m_PosterID
            >> result.m_PosterUrl
            >> result.m_PosterPicUrl
            >> result.m_PosterName
            >> posterJournalType
            >> result.m_JournalID
            >> journalType
            >> result.m_JournalName
            >> result.m_JournalUrl
            >> result.m_DItemID
            >> result.m_Subject
            >> result.m_Event
            >> result.m_PostDate
            >> result.m_Tags
            >> access
            >> props
            >> result.m_ReplyCount
            >> result.m_FullEvent
            >> result.m_ItemID
            >> result.m_CanComment
            >> result.m_Url
            >> result.m_HasArg;

    if (ver == 2)
    {
        in >> result.m_FullHasArg;
        in >> result.m_LogTime;
    }

    result.SetAccess(static_cast<Access>(access));
    result.SetJournalType(static_cast<JournalType>(journalType));
    result.SetPosterJournalType(static_cast<JournalType>(posterJournalType));
    result.SetProperties(LJEntryProperties::Deserialize(props));

    return result;
}

void LJEvent::Merge(const LJEvent& entry)
{
    if (!m_UserID)
    {
        m_UserID = entry.GetUserID();
    }
    if (!m_UserPicID)
    {
        m_UserPicID = entry.GetUserPicID();
    }
    if (m_PosterUrl.isEmpty())
    {
        m_PosterUrl = entry.GetPosterPicUrl();
    }
    if (m_PosterPicUrl.isEmpty())
    {
        m_PosterPicUrl = entry.GetPosterPicUrl();
    }
    if (m_PosterName.isEmpty())
    {
        m_PosterName = entry.GetPosterName();
    }
    if (!m_JournalID)
    {
        m_JournalID = entry.GetJournalID();
    }
    if (m_JournalName.isEmpty())
    {
        m_JournalName = entry.GetJournalName();
    }
    if (m_JournalUrl.isEmpty())
    {
        m_JournalUrl = entry.GetJournalUrl();
    }

    if (!m_DItemID)
    {
        m_DItemID = entry.GetDItemID();
    }
    if (m_Subject.isEmpty())
    {
        m_Subject = entry.GetSubject();
    }
    if (m_Event.isEmpty())
    {
        m_Event = entry.GetEvent();
    }

    if (!m_PostDate.isValid())
    {
        m_PostDate = entry.GetPostDate();
    }

    if (m_Tags.isEmpty())
    {
        m_Tags = entry.GetTags();
    }

    m_ReplyCount = entry.GetReplyCount();

    if (m_FullEvent.isEmpty())
    {
        m_FullEvent = entry.GetFullEvent();
    }
    if (!m_ItemID)
    {
        m_ItemID = entry.GetItemID();
    }

    m_CanComment = entry.IsCanComment();

    if (m_Url.isEmpty())
    {
        m_Url = entry.GetUrl();
    }

    if (m_PosterPicUrl.isEmpty())
    {
        QString picKeyword = m_Properties.GetPostAvatar();
        if (picKeyword.startsWith("pic#"))
        {
            picKeyword = picKeyword.mid(4);
            const QString avatar = QString("http://l-userpic.livejournal.com/%1/%2")
                .arg(picKeyword)
                .arg(m_JournalID);
            m_PosterPicUrl = QUrl(avatar);
        }
    }

    m_AllowMask = entry.GetAllowMask();
    if (!m_HasArg)
    {
        m_HasArg = entry.GetHasArg();
    }

    if (!m_FullHasArg)
    {
        m_FullHasArg = entry.GetFullHasArg();
    }
}

QVariantMap LJEvent::ToMap() const
{
    QVariantMap map;
    map["userId"] = m_UserID;
    map["userPicId"] = m_UserPicID;
    map["posterId"] = m_PosterID;
    map["posterUrl"] = m_PosterUrl;
    map["posterPicUrl"] = GetPosterPicUrl();
    map["posterName"] = m_PosterName;
    map["posterJournalType"] = m_PosterJournalType;
    map["journalId"] = m_JournalID;
    map["journalType"] = m_JournalType;
    map["journalName"] = m_JournalName;
    map["journalUrl"] = m_JournalUrl;
    map["dItemId"] = m_DItemID;
    map["subject"] = m_Subject;
    map["event"] = m_Event;
    map["postDate"] = m_PostDate;
    map["tags"] = m_Tags;
    map["access"] = m_Access;
    map["replyCount"] = m_ReplyCount;
    map["fullEvent"] = m_FullEvent;
    map["itemId"] = m_ItemID;
    map["canComment"] = m_CanComment;
    map["url"] = m_Url;
    map["hasArg"] = m_HasArg;
    map["fullHasArg"] = m_FullHasArg;
    map["logTime"] = m_LogTime;
    return map;
}

quint64 LJEvent::GetReplyCount() const
{
    return m_ReplyCount;
}

void LJEvent::SetReplyCount(quint64 replyCount)
{
    m_ReplyCount = replyCount;
}

QString LJEvent::GetFullEvent() const
{
    return m_FullEvent;
}

void LJEvent::SetFullEvent(const QString& event)
{
    m_FullEvent = event;
}

bool LJEvent::IsCanComment() const
{
    return m_CanComment;
}

void LJEvent::SetCanComment(bool can)
{
    m_CanComment = can;
}

quint64 LJEvent::GetPosterID() const
{
    return m_PosterID;
}

void LJEvent::SetPosterID(const quint64 posterID)
{
    m_PosterID = posterID;
}

QString LJEvent::GetPosterName() const
{
    return m_PosterName;
}

void LJEvent::SetPosterName(const QString& name)
{
    m_PosterName = name;
}

JournalType LJEvent::GetPosterJournalType() const
{
    return m_PosterJournalType;
}

void LJEvent::SetPosterJournalType(JournalType journalType)
{
    m_PosterJournalType = journalType;
}

quint64 LJEvent::GetJournalID() const
{
    return m_JournalID;
}

void LJEvent::SetJournalID(quint64 id)
{
    m_JournalID = id;
}

QUrl LJEvent::GetPosterPicUrl() const
{
    return m_PosterPicUrl.isEmpty() ?
            QUrl("qrc:/images/blank_boy.png") :
            m_PosterPicUrl;
}

void LJEvent::SetPosterPicUrl(const QUrl& url)
{
    m_PosterPicUrl = url;
}

QUrl LJEvent::GetPosterUrl() const
{
    return m_PosterUrl;
}

void LJEvent::SetPosterUrl(const QUrl& url)
{
    m_PosterUrl = url;
}

QUrl LJEvent::GetUrl() const
{
    return m_Url;
}

void LJEvent::SetUrl(const QUrl& url)
{
    m_Url = url;
}

Access LJEvent::GetAccess() const
{
    return m_Access;
}

void LJEvent::SetAccess(Access acc)
{
    m_Access = acc;
}

quint32 LJEvent::GetAllowMask() const
{
    return m_AllowMask;
}

void LJEvent::SetAllowMask(quint32 mask)
{
    m_AllowMask = mask;
}

bool LJEvent::GetHasArg() const
{
    return m_HasArg;
}

void LJEvent::SetHasArg(bool hasArg)
{
    m_HasArg = hasArg;
}

bool LJEvent::GetFullHasArg() const
{
    return m_FullHasArg;
}

void LJEvent::SetFullHasArg(bool hasArg)
{
    m_FullHasArg = hasArg;
}

int LJEvent::GetAnum() const
{
    return m_Anum;
}

void LJEvent::SetAnum(int anum)
{
    m_Anum = anum;
}

QDateTime LJEvent::GetLogTime() const
{
    return m_LogTime;
}

void LJEvent::SetLogTime(const QDateTime& dt)
{
    m_LogTime = dt;
}
} // namespace Mnemosy
