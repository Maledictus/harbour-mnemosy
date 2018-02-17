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

#include "friendssortfilterproxymodel.h"

#include <QtDebug>
#include "ljfriendsmodel.h"

namespace Mnemosy
{
FriendsSortFilterProxyModel::FriendsSortFilterProxyModel(QObject *parent)
: QSortFilterProxyModel(parent)
, m_FilterMode(FMText)
, m_SortMode(SMText)
{
    setDynamicSortFilter(true);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setSortCaseSensitivity(Qt::CaseInsensitive);
}

void FriendsSortFilterProxyModel::SetFilterMode(FilterMode mode)
{
    if (mode == m_FilterMode)
    {
        return;
    }

    m_FilterMode = mode;
    invalidateFilter();
}

void FriendsSortFilterProxyModel::SetSortMode(SortMode mode)
{
    if (mode == m_SortMode)
    {
        return;
    }

    m_SortMode = mode;
    invalidate();
}

bool FriendsSortFilterProxyModel::filterAcceptsRow(int sourceRow,
        const QModelIndex& sourceParent) const
{
    switch(m_FilterMode)
    {
    case FMText:
    {
        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        return index.data(LJFriendsModel::FRFullName).toString()
                    .contains(filterRegExp()) ||
                index.data(LJFriendsModel::FRUserName).toString()
                    .contains(filterRegExp());
    }
    case FMGroup:
        return true;
    }

    return true;
}

bool FriendsSortFilterProxyModel::lessThan(const QModelIndex& left,
        const QModelIndex& right) const
{
    switch(m_SortMode)
    {
    case SMText:
        return QString::localeAwareCompare(sourceModel()->
                    data(left, LJFriendsModel::FRUserName).toString(),
                sourceModel()->data(right, LJFriendsModel::FRUserName).toString()) < 0;
    case SMStatus:
    {
        const bool leftfrOf = sourceModel()->
                data(left, LJFriendsModel::FRFriendOf).toBool();
        const bool leftfrMy = sourceModel()->
                data(left, LJFriendsModel::FRMyFriend).toBool();
        const bool rightfrOf = sourceModel()->
                data(right, LJFriendsModel::FRFriendOf).toBool();
        const bool rightfrMy = sourceModel()->
                data(right, LJFriendsModel::FRMyFriend).toBool();

        if (leftfrOf && leftfrMy)
        {
            return true;
        }
        else if (leftfrOf)
        {
            return false;
        }
        else if (leftfrMy)
        {
            if (rightfrMy && rightfrMy)
            {
                return false;
            }
            else if (rightfrOf)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

void FriendsSortFilterProxyModel::filterFriends(int filterMode, const QString& text)
{
    SetFilterMode(static_cast<FilterMode>(filterMode));
    setFilterRegExp(text);
}

void FriendsSortFilterProxyModel::sortFriends(int sortMode, Qt::SortOrder order)
{
    SetSortMode(static_cast<SortMode>(sortMode));
    sort(0, order);
}
} // namespace Mnemosy
