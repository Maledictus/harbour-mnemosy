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

#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVariantMap>

namespace Mnemosy
{
enum Access
{
    APublic,
    AFriendsOnly,
    APrivate,
    ACustom
};

enum AdultContent
{
    ACWithoutAdultContent,
    ACAdultsFrom14,
    ACAdultsFrom18,
};

enum CommentsManagement
{
    CMShowComments,
    CMShowFriendsComments,
    CMScreenComments,
    CMScreenAnonymouseComments,
    CMScreenNotFromFriendsWithLinks,
};

enum JournalType
{
    JTPersonal,
    JTCommunity,
    JTNews,
    JTShared,
    JTSyndicated,
    JTRenamed,
    JTIdentity
};

class LJEntryProperties
{
    QString m_CurrentLocation;
    QString m_CurrentMood;
    int m_CurrentMoodId;
    QString m_CurrentMusic;
    bool m_ShowInFriendsPage;
    bool m_AutoFormat;
    AdultContent m_AdultContent;
    bool m_CommentsEnabled;
    CommentsManagement m_CommentsManagement;
    QString m_PostAvatar;
    bool m_EntryVisibility;
    bool m_NotifyByEmail;
    QStringList m_LikeButtons;

    bool m_IsRepost;
    QUrl m_RepostUrl;
    QString m_RepostedFrom;
    QString m_RepostedAuthor;
    QString m_RepostSubject;

public:
    LJEntryProperties();

    QString GetCurrentLocation() const;
    void SetCurrentLocation(const QString& currentLocation);
    QString GetCurrentMood() const;
    void SetCurrentMood(const QString& currentMood);
    int GetCurrentMoodID() const;
    void SetCurrentMoodID(int moodId);
    QString GetCurrentMusic() const;
    void SetCurrentMusic(const QString& music);
    bool GetShowInFriendsPage() const;
    void SetShowInFriendsPage(bool show);
    bool GetIsAutoFormat() const;
    void SetIsAutoFormat(bool autoFormat);
    AdultContent GetAdultContent() const;
    void SetAdultContent(AdultContent adult);
    bool GetCommentsEnabled() const;
    void SetCommentsEnabled(bool commentsEnabled);
    CommentsManagement GetCommentsManagement() const;
    void SetCommentsManagement(CommentsManagement management);
    QString GetPostAvatar() const;
    void SetPostAvatar(const QString& avatar);
    bool GetIsEntryVisible() const;
    void SetIsEntryVisible(bool visible);
    bool GetNotifyByEmail() const;
    void SetNotifyByEmail(bool visible);
    QStringList GetLikeButtons() const;
    void SetLikeButtons(const QStringList& buttons);

    bool GetIsRepost() const;
    void SetIsRepost(bool visible);
    QUrl GetRepostUrl() const;
    void SetRepostUrl(const QUrl& event);
    QString GetRepostedFrom() const;
    void SetRepostedFrom(const QString& repostedFrom);
    QString GetRepostedAuthor() const;
    void SetRepostedAuthor(const QString& repostedAuthor);
    QString GetRepostSubject() const;
    void SetRepostSubject(const QString& repostSubject);

    QByteArray Serialize() const;
    static LJEntryProperties Deserialize(const QByteArray& data);

    QVariantMap ToMap() const;
};
} // namespace Mnemosy
