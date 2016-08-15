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
    if (index.row() < 0 || index.row() > m_PostComments.m_Comments.count())
    {
        return QVariant();
    }

    LJComment comment = m_PostComments.m_Comments.at(index.row());
    switch (role)
    {
    case CRUserPicUrl:
        return comment.GetUserPicUrl();
    case CRPrivileges:
        return static_cast<quint64>(comment.GetPrivileges());
    case CRPosterID:
        return comment.GetPosterID();
    case CRState:
        return comment.GetState();
    case CRSubject:
        return comment.GetSubject();
    case CRBody:
        return comment.GetBody();
    case CRPosterPicUrl:
        return comment.GetPosterPicUrl();
    case CRDTalkID:
        return comment.GetDTalkID();
    case CRPosterName:
        return comment.GetPosterName();
    case CRDatePostUnix:
        return comment.GetDatePostUnix();
    case CRParentTalkID:
        return comment.GetParentTalkID();
    case CRLevel:
        return comment.GetLevel();
    case CRIsShow:
        return comment.IsShown();
    case CRIsLoaded:
        return comment.IsLoaded();
    case CREditTime:
        return comment.GetProperties().GetEditTime();
    case CRDeletedPoster:
        return comment.GetProperties().GetDeletedPoster();
    case CRHasArgs:
        return comment.GetHasArgs();
    default:
        return QVariant();
    }

    return QVariant();
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
    roles[CRDatePostUnix] = "commentDatePost";
    roles[CRParentTalkID] = "commentParentTalkID";
    roles[CRLevel] = "commentLevel";
    roles[CRIsShow] = "commentIsShow";
    roles[CRIsLoaded] = "commentIsLoaded";
    roles[CREditTime] = "commentEditTime";
    roles[CRDeletedPoster] = "commentDeletedPoster";

    roles[CRHasArgs] = "commentHasArgs";
    return roles;
}

int LJCommentsModel::GetCount() const
{
    return rowCount();
}

quint64 LJCommentsModel::GetCurrentPage() const
{
    return m_PostComments.m_Page;
}

quint64 LJCommentsModel::GetPagesCount() const
{
    return m_PostComments.m_Pages;
}

void LJCommentsModel::SetPostComments(const LJPostComments& postComments)
{
    beginResetModel();
    m_PostComments = postComments;
    endResetModel();
    emit countChanged();
    emit currentPageChanged();
    emit pagesCountChanged();
}

void LJCommentsModel::Clear()
{
    beginResetModel();
    m_PostComments.Reset();
    endResetModel();
    emit countChanged();
    emit currentPageChanged();
    emit pagesCountChanged();
}

void LJCommentsModel::AddComments(const LJComments_t& entries)
{

}

QVariantMap LJCommentsModel::get(int index) const
{
    return QVariantMap();
}

void LJCommentsModel::clear()
{
    Clear();
}
} // namespace Mnemosy
