/*
The MIT License(MIT)

Copyright (c) 2016-2018 Oleg Linkin <maledictusdemagog@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
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
#include <QObject>
#include <QSet>
#include <QStringList>
#include <QUrl>
#include <QVariantList>

#include "src/ljfriendsgroup.h"

namespace Mnemosy
{
class UserProfile
{

    quint64 m_UserID;
    QUrl m_DefaultPicUrl;
    QString m_UserName;
    QString m_FullName;
    QStringList m_Communities;
    QList<QPair<QString, QUrl>> m_Avatars;
    QVariantList m_AvatarsList;
    QDateTime m_Birthday;
    QSet<LJFriendGroup> m_Groups;
public:
    explicit UserProfile();

    bool IsValid() const;

    QString GetDefaultPicUrl() const;
    void SetDefaultPicUrl(const QUrl& url);
    quint64 GetUserID() const;
    void SetUserID(quint64 id);
    QString GetUserName() const;
    void SetUserName(const QString& name);
    QString GetFullName() const;
    void SetFullName(const QString& name);
    QStringList GetCommunities() const;
    QVariantList GetCommunitiesInVariant() const;
    void SetCommunities(const QStringList& list);
    QList<QPair<QString, QUrl>> GetAvatars() const;
    QVariantList GetAvatarsList() const;
    void SetAvatars(const QList<QPair<QString, QUrl>>& avatars);
    QDateTime GetBirthday() const;
    void SetBirthday(const QDateTime& dt);

    QSet<LJFriendGroup> GetFriendGroups() const;
    void SetFriendGroups(const QSet<LJFriendGroup>& groups);
    void AddFriendGroup(const LJFriendGroup& group);

    QByteArray Serialize() const;
    static bool Deserialize(const QByteArray& data, UserProfile& event);

    void UpdateProfile(const UserProfile& profile);
};
} // namespace Mnemosy
