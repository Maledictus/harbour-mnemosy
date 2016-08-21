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

#include <QtDebug>

#include "src/utils.h"

namespace Mnemosy
{
LJCommentsModel::LJCommentsModel(QObject *parent)
: QAbstractListModel(parent)
{
}

LJCommentsModel::~LJCommentsModel()
{
}

int LJCommentsModel::rowCount(const QModelIndex&) const
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
    case CRChildrenCount:
        return comment.GetChildrenCount();
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
    roles[CRPosterID] = "commentPosterId";
    roles[CRState] = "commentState";
    roles[CRSubject] = "commentSubject";
    roles[CRBody] = "commentBody";
    roles[CRPosterPicUrl] = "commentPosterPicUrl";
    roles[CRDTalkID] = "commentDTalkId";
    roles[CRPosterName] = "commentPosterName";
    roles[CRDatePostUnix] = "commentDatePost";
    roles[CRParentTalkID] = "commentParentTalkId";
    roles[CRLevel] = "commentLevel";
    roles[CRIsShow] = "commentIsShow";
    roles[CRIsLoaded] = "commentIsLoaded";
    roles[CREditTime] = "commentEditTime";
    roles[CRDeletedPoster] = "commentDeletedPoster";

    roles[CRHasArgs] = "commentHasArgs";
    roles[CRChildrenCount] = "commentChildrenCount";
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

namespace
{
void PrepareComment(LJComment& comment)
{
    bool hasArg = false;
    QString body = comment.GetBody();
    Utils::SetImagesWidth(body, hasArg);
    comment.SetHasArgs(hasArg);
    comment.SetBody(body);
}

LJComments_t ExpandComment(LJComment& comment, int level)
{
    LJComments_t comments;

    LJComments_t& children = comment.GetChildrenRef();
    for (int i = 0; i < children.count(); ++i)
    {
        LJComment child = children.at(i);
        if (child.GetLevel() - level < 4)
        {
            PrepareComment(child);
            child.SetLevel(child.GetLevel() - level);
            comments << child;
            children.takeAt(i--);
            if (child.GetChildrenCount() > 0)
            {
                comments << ExpandComment(comments.last(), level);
            }
        }
    }

    return comments;
}
}

void LJCommentsModel::SetRawPostComments(const LJPostComments& postComments)
{
    m_RawPostComments = postComments;
}

void LJCommentsModel::SetPostComments(const LJPostComments& postComments)
{
    LJComments_t list;
    for (const auto& comment : postComments.m_Comments)
    {
        LJComment tempComment = comment;
        PrepareComment(tempComment);
        list << tempComment;
        if (tempComment.GetChildrenCount() > 0)
        {
            list << ExpandComment(list.last(), list.last().GetLevel());
        }
    }
    LJPostComments comms = postComments;
    comms.m_Comments = list;

    beginResetModel();
    m_PostComments = comms;
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

QVariantMap LJCommentsModel::get(int index) const
{
    QVariantMap map;
    if (index < 0 || index > m_PostComments.m_Comments.count())
    {
        return map;
    }

    const auto& comment = m_PostComments.m_Comments.at(index);
    map["commentUserPicUrl"] = comment.GetUserPicUrl();
    map["commentPrivileges"] = static_cast<quint64>(comment.GetPrivileges());
    map["commentPosterId"] = comment.GetPosterID();
    map["commentState"] = comment.GetState();
    map["commentSubject"] = comment.GetSubject();
    map["commentBody"] = comment.GetBody();
    map["commentPosterPicUrl"] = comment.GetPosterPicUrl();
    map["commentDTalkId"] = comment.GetDTalkID();
    map["commentPosterName"] = comment.GetPosterName();
    map["commentDatePost"] = comment.GetDatePostUnix();
    map["commentParentTalkId"] = comment.GetParentTalkID();
    map["commentLevel"] = comment.GetLevel();
    map["commentIsShow"] = comment.IsShown();
    map["commentIsLoaded"] = comment.IsLoaded();
    map["commentEditTime"] = comment.GetProperties().GetEditTime();
    map["commentDeletedPoster"] = comment.GetProperties().GetDeletedPoster();
    map["commentHasArgs"] = comment.GetHasArgs();
    map["commentChildrenCount"] = comment.GetChildrenCount();

    return map;
}

int LJCommentsModel::getIndexById(quint64 id) const
{
    for (int i = 0, size = m_PostComments.m_Comments.count(); i < size; ++i)
    {
        if (m_PostComments.m_Comments.at(i).GetDTalkID() == id)
        {
            return i;
        }
    }

    return 0;
}

void LJCommentsModel::clear()
{
    Clear();
}

void LJCommentsModel::expandThread(const quint64 dTalkId)
{
    LJComment comment;
    LJPostComments postComments = m_RawPostComments;
    if (!FindComment(postComments.m_Comments, dTalkId, comment))
    {
        return;
    }

    bool found = true;
    int i = 0;
    int parentId = comment.GetParentTalkID();
    while (found && i < 3)
    {
        found = FindComment(postComments.m_Comments, parentId, comment);
        if (found)
        {
            ++i;
            parentId = comment.GetParentTalkID();
        }
    }

    if (!found)
    {
        return;
    }

    m_ParentTalkIdsHistory.push(comment.GetParentTalkID() > 0 ?
            comment.GetDTalkID() : 0);
    LoadThread(dTalkId);
}

void LJCommentsModel::collapseThread()
{
    if (!m_ParentTalkIdsHistory.empty())
    {
        LoadThread(m_ParentTalkIdsHistory.pop());
    }
}

void LJCommentsModel::LoadThread(quint64 dTalkId)
{
    if (!dTalkId)
    {
        SetPostComments(m_RawPostComments);
        return;
    }

    LJComment comment;
    LJPostComments postComments = m_RawPostComments;
    if (!FindComment(postComments.m_Comments, dTalkId, comment))
    {
        return;
    }

    LJComments_t list;
    PrepareComment(comment);
    list << comment;
    if (comment.GetChildrenCount() > 0)
    {
        list << ExpandComment(list.last(), list.last().GetLevel());
    }
    list.first().SetLevel(0);
    LJPostComments comms;
    comms.m_Comments = list;

    beginResetModel();
    m_PostComments = comms;
    endResetModel();
    emit countChanged();
}

bool LJCommentsModel::FindComment(const LJComments_t& comments, const quint64 dTalkId, LJComment& comment)
{
    bool found = false;
    for (const auto& comm : comments)
    {
        if (comm.GetDTalkID() == dTalkId)
        {
            comment = comm;
            found = true;
        }
        else if (comm.GetChildrenCount() > 0)
        {
            found = FindComment(comm.GetChildren(), dTalkId, comment);
        }

        if (found)
        {
            break;
        }
    }

    return found;
}

} // namespace Mnemosy
