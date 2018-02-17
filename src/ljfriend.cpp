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

#include "ljfriend.h"

#include <QDataStream>
#include <QtDebug>

namespace Mnemosy
{
LJFriend::LJFriend()
: m_GroupMask(0)
, m_FriendOf(false)
, m_MyFriend(false)
{
}

void LJFriend::SetAvatarUrl(const QUrl& url)
{
    m_AvatarUrl = url;
}

QUrl LJFriend::GetAvatarUrl() const
{
    return m_AvatarUrl.isEmpty() ?
            QUrl("qrc:/images/blank_boy.png") :
            m_AvatarUrl;
}

void LJFriend::SetFullName(const QString& fullName)
{
    m_FullName = fullName;
}

QString LJFriend::GetFullName() const
{
    return m_FullName;
}

void LJFriend::SetUserName(const QString& userName)
{
    m_UserName = userName;
}

QString LJFriend::GetUserName() const
{
    return m_UserName;
}

void LJFriend::SetGroupMask(int groupmask)
{
    m_GroupMask = groupmask;
}

uint LJFriend::GetGroupMask() const
{
    return m_GroupMask;
}

void LJFriend::SetBirthday(const QString& date)
{
    m_Birthday = date;
}

QString LJFriend::GetBirthday() const
{
    return m_Birthday;
}

void LJFriend::SetFriendOf(bool friendOf)
{
    m_FriendOf = friendOf;
}

bool LJFriend::GetFriendOf() const
{
    return m_FriendOf;
}

void LJFriend::SetMyFriend(bool myFriend)
{
    m_MyFriend = myFriend;
}

void LJFriend::SetFGColor(const QString& colorName)
{
    m_FGColor = colorName;
}

QString LJFriend::GetFGColor() const
{
    return m_FGColor;
}

void LJFriend::SetBGColor(const QString& colorName)
{
    m_BGColor = colorName;
}

QString LJFriend::GetBGColor() const
{
    return m_BGColor;
}

bool LJFriend::GetMyFriend() const
{
    return m_MyFriend;
}

QByteArray LJFriend::Serialize() const
{
    quint16 ver = 1;
    QByteArray result;
    {
        QDataStream ostr(&result, QIODevice::WriteOnly);
        ostr << ver
                << m_UserName
                << m_FullName
                << m_AvatarUrl
                << m_GroupMask
                << m_Birthday
                << m_FriendOf
                << m_MyFriend
                << m_BGColor
                << m_FGColor;
    }

    return result;
}

bool LJFriend::Deserialize(const QByteArray& data, LJFriend& item)
{
    quint16 ver;
    QDataStream in(data);
    in >> ver;

    if(ver != 1)
    {
        qWarning() << Q_FUNC_INFO
                << "unknown version"
                << ver;
        return false;
    }

    in >> item.m_UserName
            >> item.m_FullName
            >> item.m_AvatarUrl
            >> item.m_GroupMask
            >> item.m_Birthday
            >> item.m_FriendOf
            >> item.m_MyFriend
            >> item.m_BGColor
            >> item.m_FGColor;

    return true;
}

bool LJFriend::IsValid() const
{
    return !m_UserName.isEmpty();
}

bool LJFriend::operator==(const LJFriend& entry) const
{
    return m_UserName == entry.m_UserName;
}

LJFriend& LJFriend::operator=(const LJFriend& fr)
{
    if (this != &fr)
    {
        m_UserName = fr.GetUserName();
        m_FullName = fr.GetFullName();
        m_AvatarUrl = fr.GetAvatarUrl();
        m_GroupMask = fr.GetGroupMask();
        m_Birthday = fr.GetBirthday();
        m_BGColor = fr.GetBGColor();
        m_FGColor = fr.GetFGColor();
        m_MyFriend = fr.GetMyFriend();
    }
    return *this;
}

QVariantMap LJFriend::ToMap() const
{
    QVariantMap map;
    map["friendUserName"] = m_UserName;
    map["friendFullName"] = m_FullName;
    map["friendAvatarUrl"] = m_AvatarUrl;
    map["friendGroupMask"] = m_GroupMask;
    map["friendBirthday"] = m_Birthday;
    map["friendFriendsOf"] = m_FriendOf;
    map["friendMyFriend"] = m_MyFriend;
    map["friendBgColor"] = m_BGColor;
    map["friendFgColor"] = m_FGColor;
    return map;
}
} // namespace Mnemosy
