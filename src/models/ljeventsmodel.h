/*
The MIT License (MIT)

//Copyright (c) 2016-2017 Oleg Linkin <maledictusdemagog@gmail.com>

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

#include "src/ljevent.h"
#include "cachedmodel.h"

namespace Mnemosy
{
class LJEventsModel : public CachedModel<LJEvent>
{
    Q_OBJECT

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

        ERHasFullEntry,

        ERHasArg
    };

public:
    explicit LJEventsModel(QObject *parent = 0);
    virtual ~LJEventsModel();

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QHash<int, QByteArray> roleNames() const;

    void AddEvents(const LJEvents_t& entries);
    void UpdateEvent(const LJEvent& entry);
    void MergeEvent(const LJEvent& entry);
    void DeleteEvent(quint64 itemId);
    LJEvent GetEvent(quint64 dItemId) const;

    QDateTime GetLastItemLogTime() const;

    Q_INVOKABLE QVariantMap get(int index) const;
};
}
