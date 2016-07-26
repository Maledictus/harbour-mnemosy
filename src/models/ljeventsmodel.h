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
#include "src/ljevent.h"

namespace Mnemosy
{
class LJEventsModel : public QAbstractListModel
{
    Q_OBJECT

    LJEvents_t m_Events;

    enum LJEntryRoles
    {
        ERUserID = Qt::UserRole + 1,
        ERUserPicID,

        ERPosterID,
        ERPosterUrl,
        ERPosterPicUrl,
        ERPosterName,
        ERPosterJournalType,

        ERJournalID,
        ERJournalType,
        ERJournalName,
        ERJournalUrl,

        ERDItemID,
        ERSubject,
        EREntry,
        ERPostDate,
        ERTags,
        ERAccess,

        ERReplyCount,

        ERFullEntry,
        ERItemID,
        ERCanComment,
        ERUrl,

        ERHasFullEntry
    };

    Q_PROPERTY(int count READ GetCount NOTIFY countChanged)

public:
    explicit LJEventsModel(QObject *parent = 0);
    virtual ~LJEventsModel();

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QHash<int, QByteArray> roleNames() const;

    int GetCount() const;

    void Clear();
    void AddEvents(const LJEvents_t& entries);
    void SetEvents(const LJEvents_t& entries);
    void UpdateEvent(const LJEvent& entry);
    LJEvents_t GetEvents() const;
    LJEvent GetEvent(quint64 dItemId) const;

    QDateTime GetLastItemPostDate() const;

    Q_INVOKABLE QVariantMap get(int index) const;

signals:
    void countChanged();
};
}
Q_DECLARE_METATYPE(Mnemosy::LJEventsModel*)
// namespace Mnemosy
