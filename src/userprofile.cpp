/*
The MIT License(MIT)

Copyright (c) 2015 Oleg Linkin <maledictusdemagog@gmail.com>

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

#include "userprofile.h"

#include <QDataStream>
#include <QtDebug>

namespace Mnemosy
{
UserProfile::UserProfile()
: m_UserID(0)
{
}

bool UserProfile::IsValid() const
{
    return m_UserID > 0;
}

QString UserProfile::GetDefaultPicUrl() const
{
    return m_DefaultPicUrl.toString();
}

void UserProfile::SetDefaultPicUrl(const QUrl& url)
{
    m_DefaultPicUrl = url;
}

quint64 UserProfile::GetUserID() const
{
    return m_UserID;
}

void UserProfile::SetUserID(quint64 id)
{
    m_UserID = id;
}

QString UserProfile::GetUserName() const
{
    return m_UserName;
}

void UserProfile::SetUserName(const QString& name)
{
    m_UserName = name;
}

QString UserProfile::GetFullName() const
{
    return m_FullName;
}

void UserProfile::SetFullName(const QString& name)
{
    m_FullName = name;
}

QStringList UserProfile::GetCommunities() const
{
    return m_Communities;
}

QVariantList UserProfile::GetCommunitiesInVariant() const
{
    QVariantList list;
    for (const auto& community : m_Communities)
    {
        list.append(community);
    }
    return list;
}

void UserProfile::SetCommunities(const QStringList& list)
{
    m_Communities = list;
}

QList<QPair<QString, QUrl>> UserProfile::GetAvatars() const
{
    return m_Avatars;
}

QVariantList UserProfile::GetAvatarsList() const
{
    return m_AvatarsList;
}

void UserProfile::SetAvatars(const QList<QPair<QString, QUrl>>& avatars)
{
    m_Avatars = avatars;
    m_AvatarsList.clear();
    for (const auto& pair : m_Avatars)
    {
        QVariantMap map;
        map["avatarId"] = pair.first;
        map["avatarUrl"] = pair.second;
        m_AvatarsList << map;
    }
}

QDateTime UserProfile::GetBirthday() const
{
    return m_Birthday;
}

void UserProfile::SetBirthday(const QDateTime& dt)
{
    m_Birthday = dt;
}

QSet<LJFriendGroup> UserProfile::GetFriendGroups() const
{
    return m_Groups;
}

void UserProfile::SetFriendGroups(const QSet<LJFriendGroup>& groups)
{
    m_Groups = groups;
}

void UserProfile::AddFriendGroup(const LJFriendGroup& group)
{
    m_Groups << group;
}

QByteArray UserProfile::Serialize() const
{
    quint16 ver = 1;
    QByteArray result;
    {
        QDataStream ostr(&result, QIODevice::WriteOnly);
        ostr << ver
                << m_DefaultPicUrl
                << m_UserID
                << m_UserName
                << m_FullName
                << m_Communities
                << m_Avatars;
    }

    return result;
}

bool UserProfile::Deserialize(const QByteArray& data, UserProfile& result)
{
    quint16 ver = 0;
    QDataStream in(data);
    in >> ver;

    if(ver > 1)
    {
        qWarning() << Q_FUNC_INFO
                << "unknown version"
                << ver;
        return false;
    }

    in >> result.m_DefaultPicUrl
            >> result.m_UserID
            >> result.m_UserName
            >> result.m_FullName
            >> result.m_Communities
            >> result.m_Avatars;

    return true;
}

void UserProfile::UpdateProfile(const UserProfile& profile)
{
    SetDefaultPicUrl(profile.GetDefaultPicUrl());
    SetUserID(profile.GetUserID());
    SetUserName(profile.GetUserName());
    SetFullName(profile.GetFullName());
    SetCommunities(profile.GetCommunities());
    SetAvatars(profile.GetAvatars());
    SetBirthday(profile.GetBirthday());
    SetFriendGroups(profile.GetFriendGroups());
}
} // namespace Mnemosy
