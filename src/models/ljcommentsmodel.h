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

#pragma once

#include <QAbstractListModel>
#include <QStack>

#include "src/ljcomment.h"

namespace Mnemosy
{
class LJCommentsModel : public QAbstractListModel
{
    Q_OBJECT

    static const int FirstCommentPage = 1;

    LJPostComments m_PostComments;

    enum CommentRoles
    {
        CRUserPicUrl = Qt::UserRole + 1,
        CRPrivileges,
        CRPosterID,
        CRState,
        CRSubject,
        CRBody,
        CRPosterPicUrl,
        CRDTalkID,
        CRPosterName,
        CRDatePostUnix,
        CRParentTalkID,
        CRLevel,
        CRIsShow,
        CRIsLoaded,
        CREditTime,
        CRDeletedPoster,
        CRPictureKeyword,

        CRHasArgs,
        CRChildrenCount
    };

    Q_PROPERTY(int count READ GetCount NOTIFY countChanged)
    Q_PROPERTY(int lastLoadedPage READ GetLastLoadedPage NOTIFY lastLoadedPageChanged)
    Q_PROPERTY(int pagesCount READ GetPagesCount NOTIFY pagesCountChanged)
public:
    explicit LJCommentsModel(QObject *parent = 0);
    virtual ~LJCommentsModel();

    virtual int rowCount(const QModelIndex& parent = QModelIndex ()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QHash<int, QByteArray> roleNames() const;

    int GetCount() const;
    quint64 GetLastLoadedPage() const;
    quint64 GetPagesCount() const;

    void AddComments(const LJPostComments& postComments);
    void MarkCommentsAsDeleted(const QList<quint64>& deletedComments, const QString& posterName);
    void EditComment(const quint64 dTalkId, const QString& subject, const QString& body);
    void UpdateComments(const LJPostComments& postComments);

    void Clear();

    Q_INVOKABLE QVariantMap get(int index) const;
    Q_INVOKABLE int getIndexById(quint64 id) const;
    Q_INVOKABLE void clear();
    Q_INVOKABLE QVariantList getThread(const quint64 dTalkId);
private:
    void LoadThread(quint64 dTalkId);
    bool FindComment(const LJComments_t& comments, const quint64 dTalkId, LJComment& comment);
    void MarkCommentAsDeleted(LJComments_t& comments, const QSet<quint64>& dTalkIds,
            const QString& posterName);
    void EditComment(LJComments_t& comments, const quint64 dTalkId, const QString& subject,
            const QString& body);
    void UpdateComment(LJComments_t& comments, const LJComment& newComment);

signals:
    void countChanged();
    void lastLoadedPageChanged();
    void pagesCountChanged();

    void commentUpdated(const QVariantMap& newComment);
};
}
Q_DECLARE_METATYPE(Mnemosy::LJCommentsModel*)
