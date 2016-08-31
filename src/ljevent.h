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

#pragma once

#include <QDateTime>
#include <QString>

#include "ljeventproperties.h"

namespace Mnemosy
{
class LJEvent
{
    //Friends entry
    quint64 m_UserID;
    quint64 m_UserPicID;

    quint64 m_PosterID;
    QUrl m_PosterUrl;
    QUrl m_PosterPicUrl;
    QString m_PosterName;
    JournalType m_PosterJournalType;

    quint64 m_JournalID;
    JournalType m_JournalType;
    QString m_JournalName;
    QUrl m_JournalUrl;

    quint64 m_DItemID;
    QString m_Subject;
    QString m_Event;
    QDateTime m_PostDate;
    QStringList m_Tags;
    Access m_Access;

    LJEntryProperties m_Properties;

    quint64 m_ReplyCount;

    QString m_FullEvent;
    quint64 m_ItemID;
    bool m_CanComment;
    QUrl m_Url;
    quint32 m_AllowMask;

    bool m_HasArg;
    bool m_FullHasArg;

    int m_Anum;

public:
    LJEvent();

    bool IsValid() const;

    //Friends entry
    quint64 GetUserID() const;
    void SetUserID(quint64 id);
    quint64 GetUserPicID() const;
    void SetUserPicID(quint64 id);

    quint64 GetPosterID() const;
    void SetPosterID(const quint64 posterID);
    QUrl GetPosterUrl() const;
    void SetPosterUrl(const QUrl& url);
    QUrl GetPosterPicUrl() const;
    void SetPosterPicUrl(const QUrl& url);
    QString GetPosterName() const;
    void SetPosterName(const QString& name);
    JournalType GetPosterJournalType() const;
    void SetPosterJournalType(JournalType journalType);

    quint64 GetJournalID() const;
    void SetJournalID(quint64 id);
    JournalType GetJournalType() const;
    void SetJournalType(JournalType journalType);
    QString GetJournalName() const;
    void SetJournalName(const QString& journalName);
    QUrl GetJournalUrl() const;
    void SetJournalUrl(const QUrl& url);

    quint64 GetDItemID() const;
    void SetDItemID(quint64 ditemId);
    QString GetSubject() const;
    void SetSubject(const QString& subject);
    QString GetEvent() const;
    void SetEvent(const QString& event);
    QDateTime GetPostDate() const;
    void SetPostDate(const QDateTime& date);
    QStringList GetTags() const;
    void SetTags(const QStringList& tags);
    Access GetAccess() const;
    void SetAccess(Access acc);

    LJEntryProperties GetProperties() const;
    void SetProperties(const LJEntryProperties& properties);

    quint64 GetReplyCount() const;
    void SetReplyCount(quint64 replyCount);

    // User entry
    QString GetFullEvent() const;
    void SetFullEvent(const QString& event);

    quint64 GetItemID() const;
    void SetItemID(quint64 itemId);

    quint32 GetAllowMask() const;
    void SetAllowMask(quint32 mask);

    QUrl GetUrl() const;
    void SetUrl(const QUrl& url);

    bool IsCanComment() const;
    void SetCanComment(bool can);

    QByteArray Serialize() const;
    static LJEvent Deserialize(const QByteArray& data);

    void Merge(const LJEvent& entry);

    QVariantMap ToMap() const;

    bool GetHasArg() const;
    void SetHasArg(bool hasArg);
    bool GetFullHasArg() const;
    void SetFullHasArg(bool hasArg);

    int GetAnum() const;
    void SetAnum(int anum);
};

typedef QList<LJEvent> LJEvents_t;
}
