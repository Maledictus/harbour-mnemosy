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
    case MRJournalName:
        return msg.GetJournalName();
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
    roles[MRMsgDirection] = "messageDirection";
    roles[MRParent] = "messageParentId";
    roles[MRJournalName] = "messageJournalName";
    return roles;
}

void LJMessagesModel::AddMessages(const LJMessages_t& messages)
{
    LJMessages_t mss = messages;
    for(int i = mss.count() - 1; i >= 0; --i)
    {
        auto ms = mss.at(i);
        auto it = std::find_if(m_Items.begin(), m_Items.end(),
                [ms](decltype(m_Items.front()) message)
                {
                    return ms.GetDTalkID() == message.GetDTalkID();
                });
        if (it != m_Items.end())
        {
            const int pos = std::distance(m_Items.begin(), it);

            m_Items[pos] = ms;
            emit dataChanged(index(pos), index(pos));
        }
        else
        {
            beginInsertRows(QModelIndex(), rowCount(), rowCount());
            m_Items.append(ms);
            endInsertRows();
        }
    }
}

QList<quint64> LJMessagesModel::GetUnread() const
{
    QList<quint64> ids;
    std::transform(m_Items.begin(), m_Items.end(),
            std::back_inserter(ids),
            [](decltype(m_Items.front()) item)
            {
                return item.GetState() == LJMessage::SUnread ? item.GetQId() : 0;
            });
    ids.removeAll(0);
    return ids;
}

bool LJMessagesModel::IsUnread(const quint64 id) const
{
    auto it = std::find_if(m_Items.begin(), m_Items.end(),
            [=, &id](decltype(m_Items.front()) item)
            { return item.GetQId() == id; });
    return it == m_Items.end() || it->GetState() == LJMessage::SUnread;
}

void LJMessagesModel::MarkAsRead(const QList<quint64>& qids)
{
    for (int i = 0; i < m_Items.size(); ++i)
    {
        if (qids.contains(m_Items[i].GetQId()))
        {
            m_Items[i].SetState(LJMessage::SRead);
            emit dataChanged(index(i), index(i));
        }
    }
}

QVariantMap LJMessagesModel::get(int index) const
{
    if (index < 0 || index >= m_Items.count())
    {
        return QVariantMap();
    }

    return m_Items.at(index).ToMap();
}
} // namespace Mnemosy
