/*
The MIT License (MIT)

//Copyright (c) 2016-2017 Oleg Linkin <maledictusdemagog@gmail.com>

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

namespace Mnemosy
{
LJFriendsModel::LJFriendsModel(QObject *parent)
: QAbstractListModel(parent)
{
}

LJFriendsModel::~LJFriendsModel()
{
    m_Friends.clear();
}

QVariant LJFriendsModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() > m_Friends.count())
    {
        return QVariant();
    }

    LJFriend fr = m_Friends.at(index.row());

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
    return m_Friends.count();
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

int LJFriendsModel::GetCount() const
{
    return rowCount();
}

void LJFriendsModel::Clear ()
{
    beginResetModel();
    m_Friends.clear();
    endResetModel();
    emit countChanged();
}

QVariantMap LJFriendsModel::get(int index) const
{
    if (index < 0 || index >= m_Friends.count())
    {
        return QVariantMap();
    }

    return m_Friends.at(index).ToMap();
}

void LJFriendsModel::SetFriends(const LJFriends_t& friends)
{
    beginResetModel();
    Clear();
    m_Friends = friends;
    endResetModel();
    emit countChanged();
}

void LJFriendsModel::AddFriend(const LJFriend& fr)
{
    auto it = std::find_if(m_Friends.begin(), m_Friends.end(),
            [fr](decltype (m_Friends.front()) existFriend)
            {
                return fr.GetUserName() == existFriend.GetUserName();
            });
    if (it == m_Friends.end())
    {
        beginInsertRows(QModelIndex(), m_Friends.count(), m_Friends.count());
        m_Friends.push_back(fr);
        endInsertRows();
        emit countChanged();
    }
    else
    {
        int pos = std::distance(m_Friends.begin(), it);
        m_Friends[pos] = fr;
        emit dataChanged(index(pos), index(pos));
    }
}

LJFriends_t LJFriendsModel::GetFriends() const
{
    return m_Friends;
}

void LJFriendsModel::SetFriendAvatar(const QString& userName, const QUrl& avatar)
{
    auto it = std::find_if(m_Friends.begin(), m_Friends.end(),
        [=, &userName](decltype(m_Friends.front()) fr)
        {
            return fr.GetUserName() == userName;
        });
    if (it != m_Friends.end())
    {
        it->SetAvatarUrl(avatar);
    }
}

} // namespace Mnemosy
