/*
The MIT License(MIT)

Copyright(c) 2016-2017 Oleg Linkin <maledictusdemagog@gmail.com>

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

#include "ljfriendsgroupsmodel.h"
#include <QtDebug>

namespace Mnemosy
{
LJFriendGroupsModel::LJFriendGroupsModel(QObject *parent)
: QAbstractListModel(parent)
{
}

LJFriendGroupsModel::~LJFriendGroupsModel()
{
}

QVariant LJFriendGroupsModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() > Groups_.count())
    {
        return QVariant();
    }

    LJFriendGroup group = Groups_.at(index.row ());

    switch (role)
    {
    case FGRID:
        return group.GetId();
    case FGRName:
        return group.GetName();
    case FGRRealID:
        return group.GetRealId();
    case FGRSortOrder:
        return group.GetSortOrder();
    case FGRPublic:
        return group.IsPublic();
    default:
        return QVariant();
    }
}

int LJFriendGroupsModel::rowCount(const QModelIndex&) const
{
    return Groups_.count();
}

QHash<int, QByteArray> LJFriendGroupsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles [FGRID] = "groupId";
    roles [FGRName] = "groupName";
    roles [FGRRealID] = "groupRealId";
    roles [FGRSortOrder] = "groupSortOrder";
    roles [FGRPublic] = "groupPublic";
    return roles;
}

int LJFriendGroupsModel::GetCount() const
{
    return rowCount();
}

void LJFriendGroupsModel::Clear()
{
    beginResetModel();
    Groups_.clear();
    endResetModel();
    emit countChanged();
}

void LJFriendGroupsModel::SetGroups(const LJFriendGroups_t& groups)
{
    beginResetModel();
    Groups_ = groups;
    endResetModel();
    emit countChanged();
}
} // namespace Mnemosy
