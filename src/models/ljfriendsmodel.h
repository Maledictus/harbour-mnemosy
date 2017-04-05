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

#pragma once

#include <QVariant>
#include <QObject>

#include "src/ljfriend.h"
#include "cachedmodel.h"

namespace Mnemosy
{
class LJFriendsModel : public CachedModel<LJFriend>
{
    Q_OBJECT

    QList<LJFriend> m_Items;
public:
    enum LJFriendRoles
    {
        FRAvatar = Qt::UserRole + 1,
        FRFullName,
        FRUserName,
        FRGroupMask,
        FRBirthday,
        FRFriendOf,
        FRMyFriend,
        FRBGColor,
        FRFGColor
    };

    explicit LJFriendsModel(QObject *parent = 0);
    virtual ~LJFriendsModel();

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QHash<int, QByteArray> roleNames() const;

    void AddFriend(const LJFriend& fr);
    void EditFriend(const QString& name, const int groupMask);
    void DeleteFriend(const QString& name);

    void SetFriendAvatar(const QString& userName, const QUrl& avatar);

    void SetItems(const QList<LJFriend>& items)
    {
        beginResetModel();
        m_Items = items;
        endResetModel();
    }

    QList<LJFriend> GetItems() const
    {
        return m_Items;
    }

    void Clear()
    {
        beginResetModel();
        m_Items.clear();
        endResetModel();
    }

    int GetCount() const
    {
        return m_Items.size();
    }

    Q_INVOKABLE QVariantMap get(int index) const;
};
} // namespace Mnemosy
