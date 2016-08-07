/*
The MIT License(MIT)

//Copyright (c) 2016 Oleg Linkin <maledictusdemagog@gmail.com>

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

#include "ljcommentsmodel.h"

namespace Mnemosy
{
CommentsModel::CommentsModel(QObject *parent)
: QAbstractItemModel(parent)
{
}

CommentsModel::~CommentsModel()
{
}

QModelIndex CommentsModel::index(int row, int column, const QModelIndex& parent) const
{
    return QModelIndex();
}

QModelIndex CommentsModel::parent(const QModelIndex& child) const
{
    return QModelIndex();
}

int CommentsModel::rowCount(const QModelIndex& parent) const
{
    return 0;
}

int CommentsModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant CommentsModel::data(const QModelIndex& index, int role) const
{
    return QVariant();
}

QHash<int, QByteArray> CommentsModel::roleNames() const
{
    return QHash<int, QByteArray>();
}

int CommentsModel::GetCount() const
{
    return rowCount();
}

void CommentsModel::Clear()
{
}

void CommentsModel::AddComments(const LJComments_t& entries)
{

}

void CommentsModel::SetComments(const LJComments_t& entries)
{

}

QVariantMap CommentsModel::get(int index) const
{
    return QVariantMap();
}
} // namespace Mnemosy
