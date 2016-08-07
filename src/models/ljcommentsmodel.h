/*
The MIT License (MIT)

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

#pragma once

#include <QAbstractListModel>
#include "src/ljcomment.h"

namespace Mnemosy
{
class CommentsModel : public QAbstractItemModel
{
    Q_OBJECT

    LJComments_t m_Comments;

    enum CommentRoles
    {
        CRUserPicUrl = Qt::UserRole + 1,
        CRPriveleges,
        CRPosterID,
        CRState,
        CRSubject,
        CRBody,
        CRPosterPicUrl,
        CRDTalkID,
        CRPosterName,
        CRDatePost,
        CRDatePostUnix,
        CRParentTalkID,
        CRLevel,
        CRIsShow,
        CRIsLoaded
    };

    Q_PROPERTY(int count READ GetCount NOTIFY countChanged)
public:
    explicit CommentsModel(QObject *parent = 0);
    virtual ~CommentsModel();

    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex ()) const;
    virtual QModelIndex parent(const QModelIndex& child) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex ()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex ()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QHash<int, QByteArray> roleNames() const;

    int GetCount() const;

    void Clear();
    void AddComments(const LJComments_t& entries);
    void SetComments(const LJComments_t& entries);

    Q_INVOKABLE QVariantMap get(int index) const;

signals:
    void countChanged();
};
} // namespace Mnemosy
