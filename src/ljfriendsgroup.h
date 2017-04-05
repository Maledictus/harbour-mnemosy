/*
The MIT License(MIT)

Copyright(c) 2016-2017 Oleg Linkin <maledictusdemagog@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
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

#include <QString>
#include <QHash>

namespace Mnemosy
{
class LJFriendGroup
{
    QString m_Name;
    uint m_Id;
    uint m_SortOrder;
    uint m_RealId;
    bool m_Public;

public:
    explicit LJFriendGroup();

    QString GetName() const;
    void SetName(const QString& name);
    uint GetId() const;
    void SetId(uint id);
    uint GetSortOrder() const;
    void SetSortOrder(uint order);
    uint GetRealId() const;
    void SetRealId(uint id);
    bool IsPublic() const;
    void SetPublic(bool publicGroup);

    QByteArray Serialize() const;
    static bool Deserialize(const QByteArray& data, LJFriendGroup& item);
};

inline bool operator ==(const LJFriendGroup& left, const LJFriendGroup& right)
{
    return left.GetRealId() == right.GetRealId();
}

inline uint qHash(const LJFriendGroup& group)
{
    return ::qHash(group.GetRealId());
}

typedef QList<LJFriendGroup> LJFriendGroups_t;
} // namespace Mnemosy


