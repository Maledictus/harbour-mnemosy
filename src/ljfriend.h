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

#include <QUrl>
#include <QVariantMap>

namespace Mnemosy
{
class LJFriend
{
    QUrl m_AvatarUrl;
    QString m_FullName;
    QString m_UserName;
    uint m_GroupMask;
    QString m_Birthday;
    bool m_FriendOf;
    bool m_MyFriend;
    QString m_BGColor;
    QString m_FGColor;

public:
    LJFriend();

    void SetAvatarUrl(const QUrl& url);
    QUrl GetAvatarUrl() const;
    void SetFullName(const QString& fullName);
    QString GetFullName() const;
    void SetUserName(const QString& userName);
    QString GetUserName() const;
    void SetGroupMask(int groupmask);
    uint GetGroupMask() const;
    void SetBirthday(const QString& date);
    QString GetBirthday() const;
    void SetFriendOf(bool friendOf);
    bool GetFriendOf() const;
    void SetMyFriend(bool myFriend);
    bool GetMyFriend() const;
    QString GetBGColor() const;
    void SetBGColor(const QString& colorName);
    QString GetFGColor() const;
    void SetFGColor(const QString& colorName);

    QByteArray Serialize() const;
    static LJFriend Deserialize(const QByteArray& data);

    QVariantMap ToMap() const;

    bool operator== (const LJFriend& entry) const;
    LJFriend& operator= (const LJFriend& fr);
};

typedef QList<LJFriend> LJFriends_t;
}
