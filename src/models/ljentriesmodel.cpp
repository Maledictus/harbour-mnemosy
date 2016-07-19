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

#include "ljentriesmodel.h"

#include <algorithm>

namespace Mnemosy
{
LJEntriesModel::LJEntriesModel(QObject *parent)
: QAbstractListModel(parent)
{
}

LJEntriesModel::~LJEntriesModel()
{
    m_Entries.clear();
}

QVariant LJEntriesModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() > m_Entries.count())
    {
        return QVariant();
    }

    LJEntry entry = m_Entries.at(index.row());

    switch (role)
    {
    case ERUserID:
        return entry.GetUserID();
    case ERUserPicID:
        return entry.GetUserPicID();
    case ERPosterUrl:
        return entry.GetPosterUrl();
    case ERPosterPicUrl:
        return entry.GetPosterPicUrl();
    case ERPosterName:
        return entry.GetPosterName();
    case ERJournalID:
        return entry.GetJournalID();
    case ERJournalType:
        return entry.GetJournalType();
    case ERJournalName:
        return entry.GetJournalName();
    case ERJournalUrl:
        return entry.GetJournalUrl();
    case ERDItemID:
        return entry.GetDItemID();
    case ERSubject:
        return entry.GetSubject().isEmpty() ?
                tr("(Without subject)") :
                entry.GetSubject();
    case EREntry:
        return entry.GetEntry().isEmpty() ? entry.GetFullEntry() : entry.GetEntry();
    case ERDate:
        return entry.GetPostDate();
    case ERTags:
        return entry.GetTags().join(", ");
    case ERReplyCount:
        return entry.GetReplyCount();
    case ERFullEntry:
        return entry.GetFullEntry();
    case ERItemID:
        return entry.GetItemID();
    case ERCanComment:
        return entry.IsCanComment();
    case ERUrl:
        return entry.GetUrl();
    default:
        return QVariant();
    }
}

int LJEntriesModel::rowCount(const QModelIndex&) const
{
    return m_Entries.count();
}

QHash<int, QByteArray> LJEntriesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles [ERUserID] = "entryUserID";
    roles [ERUserPicID] = "entryUserPicID";
    roles [ERPosterUrl] = "entryPosterUrl";
    roles [ERPosterPicUrl] = "entryPosterPicUrl";
    roles [ERPosterName] = "entryPosterName";
    roles [ERJournalID] = "entryJournalID";
    roles [ERJournalType] = "entryJournalType";
    roles [ERJournalName] = "entryJournalName";
    roles [ERJournalUrl] = "entryJournalUrl";
    roles [ERDItemID] = "entryDItemID";
    roles [ERSubject] = "entrySubject";
    roles [EREntry] = "entryEntryText";
    roles [ERDate] = "entryDate";
    roles [ERTags] = "entryTags";
    roles [ERReplyCount] = "entryReplyCount";
    roles [ERFullEntry] = "entryFullEntryText";
    roles [ERItemID] = "entryItemID";
    roles [ERCanComment] = "entryCanComment";
    roles [ERUrl] = "entryUrl";

    return roles;
}

int LJEntriesModel::GetCount() const
{
    return rowCount();
}

void LJEntriesModel::Clear()
{
    beginResetModel();
    m_Entries.clear();
    endResetModel();
}

void LJEntriesModel::AddEntries(const LJEntries_t& entries)
{
    beginInsertRows(QModelIndex(), m_Entries.count(),
            m_Entries.count() + entries.count() - 1);
    m_Entries << entries;
    endInsertRows();
    emit countChanged();
}

void LJEntriesModel::SetEntries(const LJEntries_t& entries)
{
    beginResetModel();
    m_Entries = entries;
    endResetModel();
    emit countChanged();
}

void LJEntriesModel::UpdateEntry(const LJEntry& entry)
{
    auto it = std::find_if(m_Entries.begin(), m_Entries.end(),
            [entry](decltype(m_Entries.front()) oldEntry)
            { return oldEntry.GetDItemID() == entry.GetDItemID(); });
    if (it == m_Entries.end())
    {
        AddEntries({ entry });
    }
    else
    {
        int pos = std::distance(m_Entries.begin(), it);
        m_Entries [pos].Merge(entry);
        emit dataChanged(index(pos), index(pos));
    }
}

LJEntries_t LJEntriesModel::GetEntries() const
{
    return m_Entries;
}

LJEntry LJEntriesModel::GetEntry(quint64 dItemId) const
{
    auto it = std::find_if(m_Entries.begin(), m_Entries.end(),
            [dItemId](decltype(m_Entries.front()) oldEntry)
            { return oldEntry.GetDItemID() == dItemId; });
    if (it == m_Entries.end())
        return LJEntry();

    return *it;
}
} // namespace Mnemosy
