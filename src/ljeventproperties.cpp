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

#include "ljeventproperties.h"
#include <QDataStream>
#include <QtDebug>

namespace Mnemosy
{
LJEntryProperties::LJEntryProperties()
: m_CurrentMoodId(-1)
, m_ShowInFriendsPage(false)
, m_AutoFormat(false)
, m_AdultContent(ACWithoutAdultContent)
, m_CommentsEnabled(true)
, m_CommentsManagement(CMShowComments)
, m_EntryVisibility(false)
, m_NotifyByEmail(true)
, m_IsRepost(false)
{
}

QString LJEntryProperties::GetCurrentLocation() const
{
    return m_CurrentLocation;
}

void LJEntryProperties::SetCurrentLocation(const QString& currentLocation)
{
    m_CurrentLocation = currentLocation;
}

QString LJEntryProperties::GetCurrentMood() const
{
    return m_CurrentMood;
}

void LJEntryProperties::SetCurrentMood(const QString& currentMood)
{
    m_CurrentMood = currentMood;
}

int LJEntryProperties::GetCurrentMoodID() const
{
    return m_CurrentMoodId;
}

void LJEntryProperties::SetCurrentMoodID(int moodId)
{
    m_CurrentMoodId = moodId;
}

QString LJEntryProperties::GetCurrentMusic() const
{
    return m_CurrentMusic;
}

void LJEntryProperties::SetCurrentMusic(const QString& music)
{
    m_CurrentMusic = music;
}

bool LJEntryProperties::GetShowInFriendsPage() const
{
    return m_ShowInFriendsPage;
}

void LJEntryProperties::SetShowInFriendsPage(bool show)
{
    m_ShowInFriendsPage = show;
}

bool LJEntryProperties::GetIsAutoFormat() const
{
    return m_AutoFormat;
}

void LJEntryProperties::SetIsAutoFormat(bool autoFormat)
{
    m_AutoFormat = autoFormat;
}

AdultContent LJEntryProperties::GetAdultContent() const
{
    return m_AdultContent;
}

void LJEntryProperties::SetAdultContent(AdultContent adult)
{
    m_AdultContent = adult;
}

bool LJEntryProperties::GetCommentsEnabled() const
{
    return m_CommentsEnabled;
}

void LJEntryProperties::SetCommentsEnabled(bool commentsEnabled)
{
    m_CommentsEnabled = commentsEnabled;
}

CommentsManagement LJEntryProperties::GetCommentsManagement() const
{
    return m_CommentsManagement;
}

void LJEntryProperties::SetCommentsManagement(CommentsManagement management)
{
    m_CommentsManagement = management;
}

QString LJEntryProperties::GetPostAvatar() const
{
    return m_PostAvatar;
}

void LJEntryProperties::SetPostAvatar(const QString& avatar)
{
    m_PostAvatar = avatar;
}

bool LJEntryProperties::GetIsEntryVisible() const
{
    return m_EntryVisibility;
}

void LJEntryProperties::SetIsEntryVisible(bool visible)
{
    m_EntryVisibility = visible;
}

bool LJEntryProperties::GetNotifyByEmail() const
{
    return m_NotifyByEmail;
}

void LJEntryProperties::SetNotifyByEmail(bool notify)
{
    m_NotifyByEmail = notify;
}

QStringList LJEntryProperties::GetLikeButtons() const
{
    return m_LikeButtons;
}

void LJEntryProperties::SetLikeButtons(const QStringList& buttons)
{
    m_LikeButtons = buttons;
}

bool LJEntryProperties::GetIsRepost() const
{
    return m_IsRepost;
}

void LJEntryProperties::SetIsRepost(bool repost)
{
    m_IsRepost = repost;
}

QUrl LJEntryProperties::GetRepostUrl() const
{
    return m_RepostUrl;
}

void LJEntryProperties::SetRepostUrl(const QUrl& url)
{
    m_RepostUrl = url;
}

QString LJEntryProperties::GetRepostedFrom() const
{
    return m_RepostedFrom;
}

void LJEntryProperties::SetRepostedFrom(const QString& repostedFrom)
{
    m_RepostedFrom = repostedFrom;
}

QString LJEntryProperties::GetRepostedAuthor() const
{
    return m_RepostedAuthor;
}

void LJEntryProperties::SetRepostedAuthor(const QString& repostedAuthor)
{
    m_RepostedAuthor = repostedAuthor;
}

QString LJEntryProperties::GetRepostSubject() const
{
    return m_RepostSubject;
}

void LJEntryProperties::SetRepostSubject(const QString& repostSubject)
{
    m_RepostSubject = repostSubject;
}

QByteArray LJEntryProperties::Serialize() const
{
    quint16 ver = 1;
    QByteArray result;
    {
        QDataStream ostr(&result, QIODevice::WriteOnly);
        ostr << ver
                << m_CurrentLocation
                << m_CurrentMood
                << m_CurrentMoodId
                << m_CurrentMusic
                << m_ShowInFriendsPage
                << m_AutoFormat
                << m_AdultContent
                << m_CommentsEnabled
                << 0
                << m_CommentsManagement
                << 0
                << m_PostAvatar
                << m_EntryVisibility
                << m_NotifyByEmail
                << m_LikeButtons.join(',')
                << m_IsRepost
                << m_RepostUrl
                << m_RepostedFrom
                << m_RepostedAuthor
                << m_RepostSubject;
    }

    return result;
}

LJEntryProperties LJEntryProperties::Deserialize(const QByteArray& data)
{
    quint16 ver = 0;
    QDataStream in(data);
    in >> ver;

    LJEntryProperties result;
    if(ver != 1)
    {
        qWarning() << Q_FUNC_INFO
                << "unknown version"
                << ver;
        return result;
    }

    int adultContent = 0;
    int commentsManagment = 0;
    int screeningComments = 0;
    int withoutNotifications;
    QString likeButtons;
    in >> result.m_CurrentLocation
            >> result.m_CurrentMood
            >> result.m_CurrentMoodId
            >> result.m_CurrentMusic
            >> result.m_ShowInFriendsPage
            >> result.m_AutoFormat
            >> adultContent
            >> result.m_CommentsEnabled
            >> withoutNotifications
            >> commentsManagment
            >> screeningComments
            >> result.m_PostAvatar
            >> result.m_EntryVisibility
            >> result.m_NotifyByEmail
            >> likeButtons
            >> result.m_IsRepost
            >> result.m_RepostUrl
            >> result.m_RepostedFrom
            >> result.m_RepostedAuthor
            >> result.m_RepostSubject;
    result.SetAdultContent(static_cast<AdultContent>(adultContent));
    result.SetCommentsManagement(static_cast<CommentsManagement>(commentsManagment));
    result.SetLikeButtons(likeButtons.split(","));

    return result;
}

QVariantMap LJEntryProperties::ToMap() const
{
    QVariantMap map;
    map["currentLocation"] = m_CurrentLocation;
    map["currentMood"] = m_CurrentMood;
    map["currentMoodId"] = m_CurrentMoodId;
    map["currentMusic"] = m_CurrentMusic;
    map["showInFriendsPage"] = m_ShowInFriendsPage;
    map["adult"] = m_AdultContent;
    map["commentsEnabled"] = m_CommentsEnabled;
    map["notifyByEmail"] = m_NotifyByEmail;
    map["screening"] = m_CommentsManagement;
    map["postAvatar"] = m_PostAvatar;
    return map;
}
} // namespace Mnemosy
