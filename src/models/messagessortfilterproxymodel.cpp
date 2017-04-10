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

#include "messagessortfilterproxymodel.h"

#include <QtDebug>
#include "ljmessagesmodel.h"

namespace Mnemosy
{
MessagesSortFilterProxyModel::MessagesSortFilterProxyModel(QObject *parent)
: QSortFilterProxyModel(parent)
, m_Direction(LJMessage::DIn)
{
    setDynamicSortFilter(true);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void MessagesSortFilterProxyModel::SetFilterMode(const LJMessage::Direction direction)
{
    m_Direction = direction;
    invalidateFilter();
}

bool MessagesSortFilterProxyModel::filterAcceptsRow(int sourceRow,
        const QModelIndex& sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    return index.data(LJMessagesModel::MRMsgDirection).toInt() == m_Direction;
}

void Mnemosy::MessagesSortFilterProxyModel::filterMessages(int filterMode)
{
    SetFilterMode(static_cast<LJMessage::Direction>(filterMode));
}

} // namespace Mnemosy
