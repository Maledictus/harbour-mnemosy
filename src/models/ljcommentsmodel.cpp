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
LJCommentsModel::LJCommentsModel(QObject *parent)
: QAbstractListModel(parent)
{
}

LJCommentsModel::~LJCommentsModel()
{
}

int LJCommentsModel::rowCount(const QModelIndex& parent) const
{
    return m_PostComments.m_Comments.count();
}

QVariant LJCommentsModel::data(const QModelIndex& index, int role) const
{
//    if (index.row() < 0 || index.row() > m_PostComments.count())
//    {
        return QVariant();
//    }

//    CRUserPicUrl = Qt::UserRole + 1,
//    CRPrivileges,
//    CRPosterID,
//    CRState,
//    CRSubject,
//    CRBody,
//    CRPosterPicUrl,
//    CRDTalkID,
//    CRPosterName,
//    CRDatePostUnix,
//    CRParentTalkID,
//    CRLevel,
//    CRIsShow,
//    CRIsLoaded,
//    CREditTime,
//    CRDeletedPoster
}

QHash<int, QByteArray> LJCommentsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[CRUserPicUrl] = "commentUserPicUrl";
    roles[CRPrivileges] = "commentPrivileges";
    roles[CRPosterID] = "commentPosterID";
    roles[CRState] = "commentState";
    roles[CRSubject] = "commentSubject";
    roles[CRBody] = "commentBody";
    roles[CRPosterPicUrl] = "commentPosterPicUrl";
    roles[CRDTalkID] = "commentDTalkID";
    roles[CRPosterName] = "commentPosterName";
    roles[CRDatePostUnix] = "commentsDatePost";
    roles[CRParentTalkID] = "commentParentTalkID";
    roles[CRLevel] = "commentLevel";
    roles[CRIsShow] = "commentIsShow";
    roles[CRIsLoaded] = "commentIsLoaded";
    roles[CREditTime] = "commentEditTime";
    roles[CRDeletedPoster] = "commentDeletedPoster";
    return roles;
}

int LJCommentsModel::GetCount() const
{
    return rowCount();
}

void LJCommentsModel::SetPostComments(const LJPostComments& postComments)
{
    beginResetModel();
    m_PostComments = postComments;
    endResetModel();
}

void LJCommentsModel::Clear()
{
    beginResetModel();
    m_PostComments.Reset();
    endResetModel();
}

void LJCommentsModel::AddComments(const LJComments_t& entries)
{

}

QVariantMap LJCommentsModel::get(int index) const
{
    return QVariantMap();
}
} // namespace Mnemosy
