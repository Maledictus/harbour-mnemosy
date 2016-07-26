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

#include "ljentry.h"
#include <QtDebug>

namespace Mnemosy
{
LJEntry::LJEntry()
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
{
}

bool LJEntry::IsValid() const
{
    return m_ItemID || m_DItemID;
}

quint64 LJEntry::GetUserID() const
{
    return m_UserID;
}

void LJEntry::SetUserID(quint64 id)
{
    m_UserID = id;
}

quint64 LJEntry::GetUserPicID() const
{
    return m_UserPicID;
}

void LJEntry::SetUserPicID(quint64 id)
{
    m_UserPicID = id;
}

quint64 LJEntry::GetItemID() const
{
    return m_ItemID;
}

void LJEntry::SetItemID(quint64 itemId)
{
    m_ItemID = itemId;
}

quint64 LJEntry::GetDItemID() const
{
    return m_DItemID;
}

void LJEntry::SetDItemID(quint64 ditemId)
{
    m_DItemID = ditemId;
}

QString LJEntry::GetEntry() const
{
    return m_Entry;
}

void LJEntry::SetEntry(const QString& entry)
{
    m_Entry = entry;
}

QString LJEntry::GetSubject() const
{
    return m_Subject;
}

void LJEntry::SetSubject(const QString& subject)
{
    m_Subject = subject;
}

QDateTime LJEntry::GetPostDate() const
{
    return m_PostDate;
}

void LJEntry::SetPostDate(const QDateTime& date)
{
    m_PostDate = date;
}

QString LJEntry::GetJournalName() const
{
    return m_JournalName;
}

void LJEntry::SetJournalName(const QString& journalName)
{
    m_JournalName = journalName;
}

QUrl LJEntry::GetJournalUrl() const
{
    return m_JournalUrl;
}

void LJEntry::SetJournalUrl(const QUrl& url)
{
    m_JournalUrl = url;
}

QStringList LJEntry::GetTags() const
{
    return m_Tags;
}

void LJEntry::SetTags(const QStringList& tags)
{
    m_Tags = tags;
}

LJEntryProperties LJEntry::GetProperties() const
{
    return m_Properties;
}

void LJEntry::SetProperties(const LJEntryProperties& properties)
{
    m_Properties = properties;
}

JournalType LJEntry::GetJournalType() const
{
    return m_JournalType;
}

void LJEntry::SetJournalType(JournalType journalType)
{
    m_JournalType = journalType;
}

QByteArray LJEntry::Serialize() const
{
    quint16 ver = 1;
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
                << m_Entry
                << m_PostDate
                << m_Tags
                << m_Access
                << m_Properties.Serialize()
                << m_ReplyCount
                << m_FullEntry
                << m_ItemID
                << m_CanComment
                << m_Url;
    }

    return result;
}

LJEntry LJEntry::Deserialize(const QByteArray& data)
{
    quint16 ver = 0;
    QDataStream in(data);
    in >> ver;

    LJEntry result;
    if(ver != 1)
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
            >> result.m_Entry
            >> result.m_PostDate
            >> result.m_Tags
            >> access
            >> props
            >> result.m_ReplyCount
            >> result.m_FullEntry
            >> result.m_ItemID
            >> result.m_CanComment
            >> result.m_Url;
    result.SetAccess(static_cast<Access>(access));
    result.SetJournalType(static_cast<JournalType>(journalType));
    result.SetPosterJournalType(static_cast<JournalType>(posterJournalType));
    result.SetProperties(LJEntryProperties::Deserialize(props));

    return result;
}

void LJEntry::Merge(const LJEntry& entry)
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
    if (m_Entry.isEmpty())
    {
        m_Entry = entry.GetEntry();
    }
    if (m_PostDate.isValid())
    {
        m_PostDate = entry.GetPostDate();
    }
    if (m_Tags.isEmpty())
    {
        m_Tags = entry.GetTags();
    }

    m_ReplyCount = entry.GetReplyCount();

    if (m_FullEntry.isEmpty())
    {
        m_FullEntry = entry.GetFullEntry();
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
}

quint64 LJEntry::GetReplyCount() const
{
    return m_ReplyCount;
}

void LJEntry::SetReplyCount(quint64 replyCount)
{
    m_ReplyCount = replyCount;
}

QString LJEntry::GetFullEntry() const
{
    return m_FullEntry;
}

void LJEntry::SetFullEntry(const QString& entry)
{
    m_FullEntry = entry;
}

bool LJEntry::IsCanComment() const
{
    return m_CanComment;
}

void LJEntry::SetCanComment(bool can)
{
    m_CanComment = can;
}

quint64 LJEntry::GetPosterID() const
{
    return m_PosterID;
}

void LJEntry::SetPosterID(const quint64 posterID)
{
    m_PosterID = posterID;
}

QString LJEntry::GetPosterName() const
{
    return m_PosterName;
}

void LJEntry::SetPosterName(const QString& name)
{
    m_PosterName = name;
}

JournalType LJEntry::GetPosterJournalType() const
{
    return m_PosterJournalType;
}

void LJEntry::SetPosterJournalType(JournalType journalType)
{
    m_PosterJournalType = journalType;
}

quint64 LJEntry::GetJournalID() const
{
    return m_JournalID;
}

void LJEntry::SetJournalID(quint64 id)
{
    m_JournalID = id;
}

QUrl LJEntry::GetPosterPicUrl() const
{
    return m_PosterPicUrl;
}

void LJEntry::SetPosterPicUrl(const QUrl& url)
{
    m_PosterPicUrl = url;
}

QUrl LJEntry::GetPosterUrl() const
{
    return m_PosterUrl;
}

void LJEntry::SetPosterUrl(const QUrl& url)
{
    m_PosterUrl = url;
}

QUrl LJEntry::GetUrl() const
{
    return m_Url;
}

void LJEntry::SetUrl(const QUrl& url)
{
    m_Url = url;
}

Access LJEntry::GetAccess() const
{
    return m_Access;
}

void LJEntry::SetAccess(Access acc)
{
    m_Access = acc;
}

quint32 LJEntry::GetAllowMask() const
{
    return m_AllowMask;
}

void LJEntry::SetAllowMask(quint32 mask)
{
    m_AllowMask = mask;
}
} // namespace Mnemosy
