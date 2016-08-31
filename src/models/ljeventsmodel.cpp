/*
The MIT License (MIT)

Copyright (c) 2016 Oleg Linkin <maledictusdemagog@gmail.com>

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

#include "ljeventsmodel.h"

#include <algorithm>

#include <QtDebug>


namespace Mnemosy
{
LJEventsModel::LJEventsModel(QObject *parent)
: QAbstractListModel(parent)
{
}

LJEventsModel::~LJEventsModel()
{
    m_Events.clear();
}

QVariant LJEventsModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() > m_Events.count())
    {
        return QVariant();
    }

    LJEvent event = m_Events.at(index.row());

    switch (role)
    {
    case ERUserID:
        return event.GetUserID();
    case ERUserPicID:
        return event.GetUserPicID();
    case ERPosterID:
        return event.GetPosterID();
    case ERPosterUrl:
        return event.GetPosterUrl();
    case ERPosterPicUrl:
        return event.GetPosterPicUrl();
    case ERPosterName:
        return event.GetPosterName();
    case ERPosterJournalType:
        return event.GetPosterJournalType();
    case ERJournalID:
        return event.GetJournalID();
    case ERJournalType:
        return event.GetJournalType();
    case ERJournalName:
        return event.GetJournalName();
    case ERJournalUrl:
        return event.GetJournalUrl();
    case ERDItemID:
        return event.GetDItemID();
    case ERSubject:
        return event.GetSubject().isEmpty() ?
                tr("(Without subject)") :
                event.GetSubject();
    case EREntry:
        return event.GetEvent().isEmpty() ? event.GetFullEvent() : event.GetEvent();
    case ERPostDate:
        return event.GetPostDate();
    case ERTags:
        return event.GetTags().join(", ");
    case ERReplyCount:
        return event.GetReplyCount();
    case ERFullEntry:
        return event.GetFullEvent();
    case ERItemID:
        return event.GetItemID();
    case ERCanComment:
        return event.IsCanComment();
    case ERUrl:
        return event.GetUrl();
    case ERHasFullEntry:
        return !event.GetFullEvent().isEmpty();
    case ERHasArg:
        return event.GetHasArg();
    default:
        return QVariant();
    }
}

int LJEventsModel::rowCount(const QModelIndex&) const
{
    return m_Events.count();
}

QHash<int, QByteArray> LJEventsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ERUserID] = "entryUserID";
    roles[ERUserPicID] = "entryUserPicID";

    roles[ERPosterID] = "entryPosterID";
    roles[ERPosterUrl] = "entryPosterUrl";
    roles[ERPosterPicUrl] = "entryPosterPicUrl";
    roles[ERPosterName] = "entryPosterName";
    roles[ERPosterJournalType] = "entryPosterJournalType";

    roles[ERJournalID] = "entryJournalID";
    roles[ERJournalType] = "entryJournalType";
    roles[ERJournalName] = "entryJournalName";
    roles[ERJournalUrl] = "entryJournalUrl";

    roles[ERDItemID] = "entryDItemID";
    roles[ERSubject] = "entrySubject";
    roles[EREntry] = "entryEntryText";
    roles[ERPostDate] = "entryPostDate";
    roles[ERTags] = "entryTags";
    roles[ERReplyCount] = "entryReplyCount";

    roles[ERFullEntry] = "entryFullEntryText";
    roles[ERItemID] = "entryItemID";
    roles[ERCanComment] = "entryCanComment";
    roles[ERUrl] = "entryUrl";

    roles[ERHasFullEntry] = "entryHasFullText";

    roles[ERHasArg] = "entryHasArg";

    return roles;
}

int LJEventsModel::GetCount() const
{
    return rowCount();
}

void LJEventsModel::Clear()
{
    beginResetModel();
    m_Events.clear();
    endResetModel();
    emit countChanged();
}

void LJEventsModel::AddEvents(const LJEvents_t& entries)
{
    beginInsertRows(QModelIndex(), m_Events.count(),
            m_Events.count() + entries.count() - 1);
    m_Events << entries;
    endInsertRows();
    emit countChanged();
}

void LJEventsModel::SetEvents(const LJEvents_t& entries)
{
    beginResetModel();
    m_Events = entries;
    endResetModel();
    emit countChanged();
}

void LJEventsModel::UpdateEvent(const LJEvent& entry)
{
    auto it = std::find_if(m_Events.begin(), m_Events.end(),
            [entry](decltype(m_Events.front()) oldEntry)
            { return oldEntry.GetDItemID() == entry.GetDItemID(); });
    if (it == m_Events.end())
    {
        AddEvents({ entry });
    }
    else
    {
        int pos = std::distance(m_Events.begin(), it);
        m_Events[pos].Merge(entry);
        emit dataChanged(index(pos), index(pos));
    }
}

LJEvents_t LJEventsModel::GetEvents() const
{
    return m_Events;
}

LJEvent LJEventsModel::GetEvent(quint64 dItemId) const
{
    auto it = std::find_if(m_Events.begin(), m_Events.end(),
            [dItemId](decltype(m_Events.front()) oldEntry)
            { return oldEntry.GetDItemID() == dItemId; });
    if (it == m_Events.end())
        return LJEvent();

    return *it;
}

QDateTime LJEventsModel::GetLastItemLogTime() const
{
    return m_Events.last().GetLogTime();
}

QVariantMap LJEventsModel::get(int index) const
{
    if (index < 0 || index >= m_Events.count())
    {
        return QVariantMap();
    }

    return m_Events.at(index).ToMap();
}
} // namespace Mnemosy
