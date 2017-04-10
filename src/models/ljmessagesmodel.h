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

#include "src/ljmessage.h"
#include "cachedmodel.h"

namespace Mnemosy
{
class LJMessagesModel : public CachedModel<LJMessage>
{
    Q_OBJECT

public:
    enum LJMessageRoles
    {
        MRQID = Qt::UserRole + 1,

        MRPostDate,
        MRSubject,
        MRPosterId,
        MRDItemId,
        MRState,
        MRPosterAvatar,
        MRComment,
        MREntrySubject,
        MRAction,
        MRType,
        MRPosterName,
        MRBody,
        MRDTalkId,
        MRTo,
        MRToId,
        MRFrom,
        MRFromId,
        MRMsgId,
        MRMsgDirection,
        MRParent,
        MRJournalName
    };

    explicit LJMessagesModel(QObject *parent = 0);
    virtual ~LJMessagesModel();

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QHash<int, QByteArray> roleNames() const;

    void AddMessages(const LJMessages_t& messages);

    QList<quint64> GetUnread() const;
    bool IsUnread(const quint64 id) const;
    void MarkAsRead(const QList<quint64>& qids);

    Q_INVOKABLE QVariantMap get(int index) const;
};
}
