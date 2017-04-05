#include "ljfriendsgroup.h"
#include <QDataStream>
#include <QtDebug>

namespace Mnemosy
{
LJFriendGroup::LJFriendGroup()
: m_Id(0)
, m_SortOrder(0)
, m_RealId(0)
, m_Public(false)
{
}

QString LJFriendGroup::GetName() const
{
    return m_Name;
}

void LJFriendGroup::SetName(const QString& name)
{
    m_Name = name;
}

uint LJFriendGroup::GetId() const
{
    return m_Id;
}

void LJFriendGroup::SetId(uint id)
{
    m_Id = id;
}

uint LJFriendGroup::GetSortOrder() const
{
    return m_SortOrder;
}

void LJFriendGroup::SetSortOrder(uint order)
{
    m_SortOrder = order;
}

uint LJFriendGroup::GetRealId() const
{
    return m_RealId;
}

void LJFriendGroup::SetRealId(uint id)
{
    m_RealId = id;
}

void LJFriendGroup::SetPublic(bool publicGroup)
{
    m_Public = publicGroup;
}

bool LJFriendGroup::IsPublic() const
{
    return m_Public;
}

QByteArray LJFriendGroup::Serialize() const
{
    quint16 ver = 1;
    QByteArray result;
    {
        QDataStream ostr(&result, QIODevice::WriteOnly);
        ostr << ver
                << m_Id
                << m_RealId
                << m_Name
                << m_SortOrder
                << m_Public;
    }

    return result;
}

bool LJFriendGroup::Deserialize(const QByteArray& data, LJFriendGroup& item)
{
    quint16 ver = 0;
    QDataStream in (data);
    in >> ver;

    LJFriendGroup result;
    if (ver > 1)
    {
        qWarning () << Q_FUNC_INFO
                << "unknown version"
                << ver;
        return false;
    }

    in >> item.m_Id
            >> item.m_RealId
            >> item.m_Name
            >> item.m_SortOrder
            >> item.m_Public;

    return true;
}
} // namespace Mnemosy
