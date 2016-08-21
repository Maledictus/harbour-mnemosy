#include "friendsgroup.h"
#include <QDataStream>
#include <QtDebug>

namespace Mnemosy
{
FriendsGroup::FriendsGroup()
: m_Id(0)
, m_SortOrder(0)
, m_RealId(0)
, m_Public(false)
{
}

QString FriendsGroup::GetName() const
{
    return m_Name;
}

void FriendsGroup::SetName(const QString& name)
{
    m_Name = name;
}

uint FriendsGroup::GetId() const
{
    return m_Id;
}

void FriendsGroup::SetId(uint id)
{
    m_Id = id;
}

uint FriendsGroup::GetSortOrder() const
{
    return m_SortOrder;
}

void FriendsGroup::SetSortOrder(uint order)
{
    m_SortOrder = order;
}

uint FriendsGroup::GetRealId() const
{
    return m_RealId;
}

void FriendsGroup::SetRealId(uint id)
{
    m_RealId = id;
}

void FriendsGroup::SetPublic(bool publicGroup)
{
    m_Public = publicGroup;
}

bool FriendsGroup::IsPublic() const
{
    return m_Public;
}

QByteArray FriendsGroup::Serialize() const
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

FriendsGroup FriendsGroup::Deserialize(const QByteArray& data)
{
    quint16 ver = 0;
    QDataStream in (data);
    in >> ver;

    FriendsGroup result;
    if (ver > 1)
    {
        qWarning () << Q_FUNC_INFO
                << "unknown version"
                << ver;
        return result;
    }

    in >> result.m_Id
            >> result.m_RealId
            >> result.m_Name
            >> result.m_SortOrder
            >> result.m_Public;

    return result;
}
} // namespace Mnemosy
