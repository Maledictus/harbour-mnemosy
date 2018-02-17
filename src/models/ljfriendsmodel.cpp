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

#include "ljfriendsmodel.h"

#include <QColor>
#include <QtDebug>

namespace Mnemosy
{
LJFriendsModel::LJFriendsModel(QObject *parent)
: CachedModel(parent)
{
}

LJFriendsModel::~LJFriendsModel()
{
}

QVariant LJFriendsModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() > m_Items.count())
    {
        return QVariant();
    }

    LJFriend fr = m_Items.at(index.row());

    switch (role)
    {
    case FRAvatar:
        return fr.GetAvatarUrl();
    case FRFullName:
        return fr.GetFullName();
    case FRUserName:
        return fr.GetUserName();
    case FRGroupMask:
        return fr.GetGroupMask();
    case FRBirthday:
        return fr.GetBirthday();
    case FRFriendOf:
        return fr.GetFriendOf();
    case FRMyFriend:
        return fr.GetMyFriend();
    case FRBGColor:
        return QColor(fr.GetBGColor());
    case FRFGColor:
        return QColor(fr.GetFGColor());
    default:
        return QVariant();
    }

    return QVariant();
}

int LJFriendsModel::rowCount(const QModelIndex&) const
{
    return GetCount();
}

QHash<int, QByteArray> LJFriendsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles [FRAvatar] = "friendAvatar";
    roles [FRFullName] = "friendFullName";
    roles [FRUserName] = "friendUserName";
    roles [FRGroupMask] = "friendGroupMask";
    roles [FRBirthday] = "friendBirthday";
    roles [FRFriendOf] = "friendOfMe";
    roles [FRMyFriend] = "friendMy";
    roles [FRBGColor] = "friendBGColor";
    roles [FRFGColor] = "friendFGColor";

    return roles;
}

void LJFriendsModel::AddFriend(const LJFriend& fr)
{
    auto it = std::find_if(m_Items.begin(), m_Items.end(),
            [fr](decltype (m_Items.front()) existFriend)
            {
                return fr.GetUserName() == existFriend.GetUserName();
            });
    if (it == m_Items.end())
    {
        beginInsertRows(QModelIndex(), m_Items.count(), m_Items.count());
        m_Items.push_back(fr);
        endInsertRows();
    }
    else
    {
        const int pos = std::distance(m_Items.begin(), it);
        m_Items[pos] = fr;
        emit dataChanged(index(pos), index(pos));
    }
}

void LJFriendsModel::EditFriend(const QString& name, const int groupMask)
{
    auto it = std::find_if(m_Items.begin(), m_Items.end(),
            [name](decltype (m_Items.front()) existFriend)
            {
                return existFriend.GetUserName() == name;
            });
    if (it == m_Items.end())
    {
        qDebug() << "There is no friend with name " << name;
        return;
    }
    const int pos = std::distance(m_Items.begin(), it);
    m_Items[pos].SetGroupMask(groupMask);
    emit dataChanged(index(pos), index(pos), { FRGroupMask });
}

void LJFriendsModel::DeleteFriend(const QString& name)
{
    auto it = std::find_if(m_Items.begin(), m_Items.end(),
            [name](decltype (m_Items.front()) existFriend)
            {
                return existFriend.GetUserName() == name;
            });
    if (it == m_Items.end())
    {
        qDebug() << "There is no friend with name " << name;
        return;
    }
    const int pos = std::distance(m_Items.begin(), it);
    auto& fr = m_Items[pos];
    if (fr.GetFriendOf() && fr.GetMyFriend())
    {
        fr.SetMyFriend(false);
        dataChanged(index(pos), index(pos), { FRMyFriend });
    }
    else if (fr.GetMyFriend())
    {
        beginRemoveRows(QModelIndex(), pos, pos);
        m_Items.removeAt(pos);
        endRemoveRows();
    }
}

void LJFriendsModel::SetFriendAvatar(const QString& userName, const QUrl& avatar)
{
    auto it = std::find_if(m_Items.begin(), m_Items.end(),
        [=, &userName](decltype(m_Items.front()) fr)
        {
            return fr.GetUserName() == userName;
        });
    if (it != m_Items.end())
    {
        it->SetAvatarUrl(avatar);
    }
}

QVariantMap LJFriendsModel::get(int index) const
{
    if (index < 0 || index >= m_Items.count())
    {
        return QVariantMap();
    }

    return m_Items.at(index).ToMap();
}
} // namespace Mnemosy
