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

#include <QAbstractListModel>
#include <QHash>

#include "src/ljfriendsgroup.h"

namespace Mnemosy
{
class LJFriendGroupsModel : public QAbstractListModel
{
    Q_OBJECT

    LJFriendGroups_t Groups_;

    enum LJFriendGroupRoles
    {
        FGRID = Qt::UserRole + 1,
        FGRName,
        FGRRealID,
        FGRSortOrder,
        FGRPublic
    };

    Q_PROPERTY(int count READ GetCount NOTIFY countChanged)
public:

    explicit LJFriendGroupsModel(QObject *parent = 0);
    virtual ~LJFriendGroupsModel();

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QHash<int, QByteArray> roleNames() const;

    int GetCount() const;

    void Clear();
    void SetGroups(const LJFriendGroups_t& groups);

signals:
    void countChanged();
};
} // namespace Mnemosy

Q_DECLARE_METATYPE(Mnemosy::LJFriendGroupsModel*)
