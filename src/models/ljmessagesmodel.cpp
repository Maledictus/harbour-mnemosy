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

#include "ljmessagesmodel.h"

#include <algorithm>

#include <QtDebug>


namespace Mnemosy
{
LJMessagesModel::LJMessagesModel(QObject *parent)
: CachedModel(parent)
{
}

LJMessagesModel::~LJMessagesModel()
{
    Clear();
}

QVariant LJMessagesModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() > m_Items.count())
    {
        return QVariant();
    }

    LJMessage msg = m_Items.at(index.row());

    switch (role)
    {
    case MRQID:
        return msg.GetQId();
    case MRPostDate:
        return msg.GetDate();
    case MRSubject:
        return msg.GetSubject();
    case MRPosterId:
        return msg.GetPosterId();
    case MRDItemId:
        return msg.GetEntryDItemId();
    case MRState:
        return msg.GetState();
    case MRPosterAvatar:
        return msg.GetPosterPicUrl();
    case MRComment:
        return msg.GetCommentUrl();
    case MREntrySubject:
        return msg.GetEntrySubject();
    case MRAction:
        return msg.GetAction();
    case MRType:
        return msg.GetType();
    case MRPosterName:
        return msg.GetPoster();
    case MRBody:
        return msg.GetBody();
    case MRDTalkId:
        return msg.GetDTalkID();
    case MRTo:
        return msg.GetTo();
    case MRToId:
        return msg.GetToId();
    case MRFrom:
        return msg.GetFrom();
    case MRFromId:
        return msg.GetFromId();
    case MRMsgId:
        return msg.GetMessageId();
    case MRMsgDirection:
        return msg.GetDirection();
    case MRParent:
        return msg.GetParentID();
    default:
        return QVariant();
    }
}

int LJMessagesModel::rowCount(const QModelIndex&) const
{
    return m_Items.count();
}

QHash<int, QByteArray> LJMessagesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[MRQID] = "messageQId";
    roles[MRPostDate] = "messagePostDate";
    roles[MRSubject] = "messageSubject";
    roles[MRPosterId] = "messagePosterId";
    roles[MRDItemId] = "messageEntryDItemId";
    roles[MRState] = "messageState";
    roles[MRPosterAvatar] = "messagePosterAvatar";
    roles[MRComment] = "messageCommentUrl";
    roles[MREntrySubject] = "messageEntrySubject";
    roles[MRAction] = "messageAction";
    roles[MRType] = "messageType";
    roles[MRPosterName] = "messagePosterName";
    roles[MRBody] = "messageBody";
    roles[MRDTalkId] = "messageDTalkId";
    roles[MRTo] = "messageTo";
    roles[MRToId] = "messageToId";
    roles[MRFrom] = "messageFrom";
    roles[MRFromId] = "messageFromId";
    roles[MRMsgId] = "messageId";
    roles[MRMsgDirection] = "messageDirections";
    roles[MRParent] = "messageParentId";
    return roles;
}

void LJMessagesModel::AddMessages(const LJMessages_t& messages)
{
    beginInsertRows(QModelIndex(), m_Items.count(),
            m_Items.count() + messages.count() - 1);
    m_Items << messages;
    endInsertRows();
}
} // namespace Mnemosy
