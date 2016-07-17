/*
The MIT License(MIT)

Copyright(c) 2016 Oleg Linkin <maledictusdemagog@gmail.com>

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

#include "rpcutils.h"
//#include <QGuiApplication>
//#include <QScreen>
#include <QtDebug>

#include "src/friendsgroup.h"

namespace Mnemosy
{
namespace RpcUtils
{
namespace Builder
{
    QPair<QDomElement, QDomElement> GetStartPart(const QString& name,
            QDomDocument doc)
    {
        QDomElement methodCall = doc.createElement("methodCall");
        doc.appendChild(methodCall);
        QDomElement methodName = doc.createElement("methodName");
        methodCall.appendChild(methodName);
        QDomText methodNameText = doc.createTextNode(name);
        methodName.appendChild(methodNameText);
        QDomElement params = doc.createElement("params");
        methodCall.appendChild(params);
        QDomElement param = doc.createElement("param");
        params.appendChild(param);
        QDomElement value = doc.createElement("value");
        param.appendChild(value);
        QDomElement structElem = doc.createElement("struct");
        value.appendChild(structElem);

        return { methodCall, structElem };
    }

    QDomElement GetSimpleMemberElement(const QString& nameVal,
            const QString& typeVal, const QString& value, QDomDocument doc)
    {
        QDomElement member = doc.createElement("member");
        QDomElement name = doc.createElement("name");
        member.appendChild(name);
        QDomText nameText = doc.createTextNode(nameVal);
        name.appendChild(nameText);
        QDomElement valueType = doc.createElement("value");
        member.appendChild(valueType);
        QDomElement type = doc.createElement(typeVal);
        valueType.appendChild(type);
        QDomText text = doc.createTextNode(value);
        type.appendChild(text);

        return member;
    }

    QPair<QDomElement, QDomElement> GetComplexMemberElement(const QString& nameVal,
            const QString& typeVal, QDomDocument doc)
    {
        QDomElement member = doc.createElement("member");
        QDomElement name = doc.createElement("name");
        member.appendChild(name);
        QDomText nameText = doc.createTextNode(nameVal);
        name.appendChild(nameText);
        QDomElement valueType = doc.createElement("value");
        member.appendChild(valueType);
        QDomElement type = doc.createElement(typeVal);
        valueType.appendChild(type);
        QDomElement dataField;
        if (typeVal == "array")
        {
            dataField = doc.createElement("data");
            type.appendChild(dataField);
        }
        else if (typeVal == "struct")
            dataField = type;

        return { member, dataField };
    }

    QDomElement FillServicePart(QDomElement parentElement,
            const QString& login, const QString& password,
            const QString& challenge, QDomDocument document)
    {
        parentElement.appendChild(GetSimpleMemberElement("auth_method", "string",
                "challenge", document));
        parentElement.appendChild(GetSimpleMemberElement("auth_challenge", "string",
                challenge, document));
        parentElement.appendChild(GetSimpleMemberElement("username", "string",
                login, document));
        parentElement.appendChild(GetSimpleMemberElement("auth_response", "string",
                password, document));
        parentElement.appendChild(GetSimpleMemberElement("ver", "int",
                "1", document));

        return parentElement;
    }
}

namespace Parser
{
LJParserType ParseMember(const QDomNode& node)
{
    const auto& memberFields = node.childNodes();
    const auto& memberNameField = memberFields.at(0);
    const auto& memberValueField = memberFields.at(1);
    QString memberName;
    QVariantList memberValue;
    if (memberNameField.isElement() &&
            memberNameField.toElement().tagName() == "name")
        memberName = memberNameField.toElement().text();

    if (memberValueField.isElement())
        memberValue = ParseValue(memberValueField);

    return LJParserType(memberName, memberValue);
}

QVariantList ParseValue(const QDomNode& node)
{
    QVariantList result;
    const auto& valueNode = node.firstChild();
    const auto& valueElement = valueNode.toElement();
    QString type = valueElement.tagName();
    if (type == "string" ||
            type == "int" ||
            type == "i4" ||
            type == "double" ||
            type == "boolean")
        result << valueElement.text();
    else if (type == "dateTime.iso8601")
        result << QDateTime::fromString(valueElement.text(), Qt::ISODate);
    else if (type == "base64")
        result << QString::fromUtf8(QByteArray::fromBase64(valueElement
                .text().toUtf8()));
    else if (type == "array")
    {
        const auto& arrayElements = valueNode.firstChild().childNodes();
        QVariantList array;
        for(int i = 0, count = arrayElements.count(); i < count; ++i)
            array << QVariant::fromValue<QVariantList>(ParseValue(arrayElements.at(i)));

        result << array;
    }
    else if (type == "struct")
    {
        const auto& structMembers = valueNode.childNodes();
        for(int i = 0, count = structMembers.count(); i < count; ++i)
            result << QVariant::fromValue<LJParserType>(ParseMember(structMembers.at(i)));
    }

    return result;
}

namespace
{
FriendsGroup CreateGroup(const QVariantList& data)
{
    FriendsGroup group;
    for (const auto& field : data)
    {
        auto fieldEntry = field.value<LJParserType>();
        if (fieldEntry.Name() == "name")
        {
            group.SetName(fieldEntry.ValueToString());
        }
        else if (fieldEntry.Name() == "id")
        {
            group.SetId(fieldEntry.ValueToInt());
            group.SetRealId((1 << group.GetId()) + 1);
        }
        else if (fieldEntry.Name () == "sortorder")
        {
            group.SetSortOrder(fieldEntry.ValueToInt());
        }
        else if (fieldEntry.Name() == "public")
            group.SetPublic(fieldEntry.ValueToInt());
    }

    return group;
}
}

struct Id2ProfileField
{
    QHash<QString, std::function<void(UserProfile*, const LJParserType&)>> Id2ProfileField_;

    Id2ProfileField()
    {
        Id2ProfileField_["defaultpicurl"] = [](UserProfile *profile,
                const LJParserType& entry)
        {
            profile->SetDefaultPicUrl(entry.ValueToUrl());
        };

        Id2ProfileField_["usejournals"] = [](UserProfile *profile,
                const LJParserType& entry)
        {
            QStringList communities;
            for(const auto& val : entry.Value())
            {
                communities << val.toList().value(0).toString();
            }
            profile->SetCommunities(communities);
        };

        Id2ProfileField_["fullname"] = [](UserProfile *profile,
                const LJParserType& entry)
        {
            profile->SetFullName(entry.ValueToString());
        };

        Id2ProfileField_["userid"] = [](UserProfile *profile,
                const LJParserType& entry)
        {
            profile->SetUserID(entry.ValueToLongLong());
        };

        Id2ProfileField_["username"] = [](UserProfile *profile,
                const LJParserType& entry)
        {
            profile->SetUserName(entry.ValueToString());
        };

        Id2ProfileField_["pickws"] = [](UserProfile *profile,
                const LJParserType& entry)
        {
            auto list = profile->GetAvatars();
            if (list.isEmpty())
            {
                for(const auto& val : entry.Value())
                {
                    list << qMakePair<QString, QUrl>(val.toList()
                            .value(0).toString(), QUrl());
                }
            }
            else
            {
                const auto& valList = entry.Value();
                for(int i = 0, count = list.size(); i < count; ++i)
                {
                    list[i].first = valList.at(i).toList()
                            .value(0).toString();
                }
            }
            profile->SetAvatars(list);
        };

        Id2ProfileField_["pickwurls"] = [](UserProfile *profile,
                const LJParserType& entry)
        {
            auto list = profile->GetAvatars();
            if (list.isEmpty())
            {
                for(const auto& val : entry.Value())
                {
                    list << qMakePair<QString, QUrl>(QString(),
                            QUrl(val.toList().value(0).toString()));
                }
            }
            else
            {
                const auto& valList = entry.Value();
                for(int i = 0, count = list.size(); i < count; ++i)
                {
                    list[i].second = QUrl(valList.at(0).toList()
                            .value(0).toString());
                }
            }
            profile->SetAvatars(list);
        };

        Id2ProfileField_["friendgroups"] = [](UserProfile *profile,
                const LJParserType& entry)
        {
              for(const auto& friendGroupEntry : entry.Value())
              {
                  profile->AddFriendsGroup(CreateGroup(friendGroupEntry.toList()));
              }
        };
    }
};

UserProfile* ParseUserProfile(const QDomDocument& document)
{
    static Id2ProfileField id2field;
    const auto& firstStructElement = document.elementsByTagName("struct");
    if (firstStructElement.at(0).isNull())
    {
        return nullptr;
    }

    UserProfile *profile = new UserProfile();
    const auto& members = firstStructElement.at(0).childNodes();
    for(int i = 0, count = members.count(); i < count; ++i)
    {
        const QDomNode& member = members.at(i);
        if (!member.isElement() ||
                member.toElement().tagName() != "member")
            continue;

        auto res = ParseMember(member);
        if (id2field.Id2ProfileField_.contains(res.Name()))
        {
            id2field.Id2ProfileField_ [res.Name()](profile, res);
        }
    }

    return profile;
}

//    namespace
//    {
//        Access GetAccessForString(const QString& access)
//        {
//            if (access == "private")
//                return APrivate;
//            else if (access == "usemask")
//                return ACustom;
//            else
//                return APublic;
//        }

//        AdultContent GetAdultContentFromString(const QString& str)
//        {
//            if (str == "concepts")
//                return ACAdultsFrom14;
//            else if (str == "explicit")
//                return ACAdultsFrom18;
//            else
//                return ACWithoutAdultContent;
//        }

//        CommentsManagement GetCommentsManagmentFromString(const QString& str)
//        {
//            CommentsManagement cm = CMDefault;
//            if (str == "N")
//                cm = CMShowComments;
//            else if (str == "R")
//                cm = CMScreenAnonymouseComments;
//            else if (str == "F")
//                cm = CMShowFriendsComments;
//            else if (str == "L")
//                cm = CMScreenNotFromFriendsWithLinks;
//            else if (str == "A")
//                cm = CMScreenComments;

//            return cm;
//        }

//        CommentsManagement GetCommentsManagmentFromInt(int cm)
//        {
//            switch(cm)
//            {
//            case 1:
//                return CMDisableComments;
//            case 0:
//            default:
//                return CMEnableComments;
//            }
//        }

//        LJEntryProperties CreateLJEntryPropetries(QStringList& tags,
//                const QVariantList& data)
//        {
//            LJEntryProperties props;
//            for(const auto& prop : data)
//            {
//                auto propsFieldEntry = prop.value<LJParserType>();
//                if (propsFieldEntry.Name() == "picture_keyword")
//                    props.SetPostAvatar(propsFieldEntry.ValueToString());
//                else if (propsFieldEntry.Name() == "opt_screening")
//                    props.SetCommentsManagement(GetCommentsManagmentFromString(propsFieldEntry.ValueToString()));
//                else if (propsFieldEntry.Name() == "current_music")
//                    props.SetCurrentMusic(propsFieldEntry.ValueToString());
//                else if (propsFieldEntry.Name() == "current_mood")
//                    props.SetCurrentMood(propsFieldEntry.ValueToString());
//                else if (propsFieldEntry.Name() == "current_location")
//                    props.SetCurrentLocation(propsFieldEntry.ValueToString());
//                else if (propsFieldEntry.Name() == "taglist")
//                    tags = propsFieldEntry.ValueToString().split(", ");
//                else if (propsFieldEntry.Name() == "adult_content")
//                    props.SetAdultContent(GetAdultContentFromString(propsFieldEntry.ValueToString()));
//                else if (propsFieldEntry.Name() == "opt_nocomments")
//                    props.SetCommentsManagement(GetCommentsManagmentFromInt(propsFieldEntry.ValueToInt()));
//                else if (propsFieldEntry.Name() == "repost")
//                    props.SetIsRepost((propsFieldEntry.ValueToString() == "c"));
//                else if (propsFieldEntry.Name() == "repost_url")
//                    props.SetRepostUrl(propsFieldEntry.ValueToUrl());
//            }

//            return props;
//        }

//        QString PrepareEvent(QString event)
//        {
//            event.replace("\n", "<br />");
//            event.replace("&laquo;", "«");
//            event.replace("&raquo;", "»");
//            event.replace("&nbsp;", " ");
//            event.replace("&nbsp", " ");

//            const int screenWidth = QGuiApplication::primaryScreen()->
//                    availableSize().width();
//            QRegExp imgRxp ("\\<img[^\\>]*src\\s*=\\s*\"[^\"]*\"[^\\>]*\\>",
//                    Qt::CaseInsensitive);
//            imgRxp.setMinimal(true);
//            int offset = 0;
//            QList<std::tuple<QString, QString, int>> matched;
//            while ((offset = imgRxp.indexIn(event, offset)) != -1)
//            {
//                QString imgTag = imgRxp.cap(0);
//                if (!imgTag.contains("l-stat.livejournal.net"))
//                {
//                    QRegExp urlRxp("src\\s*=\\s*[\"']([^\"]*)[\"']");
//                    QString url;
//                    if (urlRxp.indexIn(imgTag) != -1)
//                        url = urlRxp.cap(1);
//                    int width = 0;
//                    QRegExp widthRxp("width\\s*=\\s*[\"'](\\d+)[\"']");
//                    if (widthRxp.indexIn(imgTag) != -1)
//                        width = widthRxp.cap(1).toInt ();

//                    matched << std::make_tuple(imgTag, url, width);
//                }
//                offset += imgRxp.matchedLength();
//            }

//            for (const auto& t : matched)
//            {
//                event.replace (std::get<0>(t),
//                        "<img src=\"" + std::get<1>(t) + QString("\" width=\"%1\" />").arg(screenWidth - 6));
//            }
//            return event;
//        }

//        QString SdelanoUNasEntryPreparing(QString entry)
//        {
//            QRegExp listRxp ("\\<ul\\s*style=\\\"list-style:\\s*.*;.*\\\"\\>"
//                    "\\<li\\>\\s*\\<!--noindex--\\>"
//                    "\\<a\\s*.*href=\\\".*sdelanounas\\.ru.*\\\".*\\>"
//                    "(\\<img[^\\>]*src\\s*=\\s*\"[^\"]*\"[^\\>]*\\/\\>)"
//                    "\\<\\/a\\>.*\\<\\/ul\\>", Qt::CaseInsensitive);
//            listRxp.setMinimal(true);
//            entry.replace(listRxp, "\\1");

//            return entry;
//        }

//        QString PrepareSubject(QString subject)
//        {
//            subject.replace("&laquo;", "«");
//            subject.replace("&raquo;", "»");
//            subject.replace("&nbsp;", " ");
//            subject.replace("&nbsp", " ");
//            subject.replace("&#8470;", "№");
//            subject.replace("&mdash;", "—");
//            subject.replace("&thinsp;", " ");
//            return subject;
//        }

//        LJEntry CreateLJEntry(const QVariant& data, const QVariantMap& params)
//        {
//            LJEntry entry;
//            const QString journalId = params.value(JournalIDKey).toString();
//            const bool isUserJournal = params.value(IsUserJournalKey).toBool();
//            for(const auto& field : data.toList())
//            {
//                auto fieldEntry = field.value<LJParserType>();
//                //Friends entry
//                if (fieldEntry.Name() == "userpic")
//                {
//                    for (const auto& userpicField : fieldEntry.Value())
//                    {
//                        auto userpicFieldEntry = userpicField.value<LJParserType>();
//                        if (userpicFieldEntry.Name() == "picid")
//                            entry.SetUserPicID(userpicFieldEntry.ValueToLongLong());
//                        else if (userpicFieldEntry.Name() == "userid")
//                            entry.SetUserID(userpicFieldEntry.ValueToLongLong());
//                    }
//                }
//                else if (fieldEntry.Name() == "posterurl")
//                    entry.SetPosterUrl(fieldEntry.ValueToUrl());
//                else if (fieldEntry.Name() == "poster_userpic_url")
//                    entry.SetPosterPicUrl(fieldEntry.ValueToUrl());
//                else if (fieldEntry.Name() == "postername")
//                    entry.SetPosterName(fieldEntry.ValueToString());
//                else if (fieldEntry.Name() == "journalid")
//                    entry.SetJournalID(fieldEntry.ValueToLongLong());
//                else if (fieldEntry.Name() == "journaltype")
//                {
//                    switch(fieldEntry.ValueToString()[0].toLatin1())
//                    {
//                    case 'P':
//                        entry.SetJournalType(JTPersonal);
//                        break;
//                    case 'C':
//                        entry.SetJournalType(JTCommunity);
//                        break;
//                    case 'N':
//                        entry.SetJournalType(JTNews);
//                        break;
//                    case 'S':
//                        entry.SetJournalType(JTShared);
//                        break;
//                    case 'Y':
//                        entry.SetJournalType(JTSyndicated);
//                        break;
//                    case 'R':
//                        entry.SetJournalType(JTRenamed);
//                        break;
//                    case 'I':
//                        entry.SetJournalType(JTIdentity);
//                        break;
//                    default:
//                        break;
//                    }
//                }
//                else if (fieldEntry.Name() == "journalname")
//                    entry.SetJournalName(fieldEntry.ValueToString());
//                else if (fieldEntry.Name() == "journalurl")
//                    entry.SetJournalUrl(fieldEntry.ValueToUrl());
//                else if (fieldEntry.Name() == "ditemid")
//                    entry.SetDItemID(fieldEntry.ValueToLongLong());
//                else if (fieldEntry.Name() == "subject" ||
//                        fieldEntry.Name() == "subject_raw")
//                    entry.SetSubject(PrepareSubject(fieldEntry.ValueToString()));
//                else if (fieldEntry.Name() == "event")
//                {
//                    if (!isUserJournal)
//                        entry.SetFullEntry(PrepareEvent(fieldEntry.ValueToString()));
//                    else
//                        entry.SetEntry(PrepareEvent(fieldEntry.ValueToString()));
//                }
//                else if (fieldEntry.Name() == "event_raw")
//                    entry.SetEntry(PrepareEvent(fieldEntry.ValueToString()));
//                else if (fieldEntry.Name() == "logtime" ||
//                        fieldEntry.Name() == "event_timestamp")
//                {
//                    QDateTime dt = QDateTime::fromTime_t(fieldEntry.ValueToLongLong());
//                    if (dt.isValid())
//                        entry.SetPostDate(dt);
//                }
//                else if (fieldEntry.Name() == "props")
//                {
//                    QStringList tags;
//                    entry.SetProperties(CreateLJEntryPropetries(tags,
//                            fieldEntry.Value()));
//                    entry.SetTags(tags);
//                    if (entry.IsRealPosterPicUrlEmpty() && !journalId.isEmpty())
//                    {
//                        QString picKeyword = entry.GetProperties().GetPostAvatar();
//                        if (picKeyword.startsWith("pic#"))
//                        {
//                            picKeyword = picKeyword.mid(4);
//                            const QString avatar = QString("http://l-userpic.livejournal.com/%1/%2")
//                                .arg(picKeyword)
//                                .arg(journalId);
//                            entry.SetPosterPicUrl(QUrl(avatar));
//                        }
//                    }
//                }
//                else if (fieldEntry.Name() == "reply_count")
//                    entry.SetReplyCount(fieldEntry.ValueToInt());
//                else if (fieldEntry.Name() == "security")
//                    entry.SetAccess(GetAccessForString(fieldEntry.ValueToString()));
//                else if (fieldEntry.Name() == "itemid")
//                    entry.SetItemID(fieldEntry.ValueToLongLong());
//                else if (fieldEntry.Name() == "can_comment")
//                    entry.SetCanComment(fieldEntry.ValueToInt());
//                else if (fieldEntry.Name() == "url")
//                    entry.SetUrl(QUrl(fieldEntry.ValueToUrl()));

////				else if (fieldEntry.Name() == "anum")
////					entry.SetANum(fieldEntry.ValueToInt());
//////				else if (fieldEntry.Name() == "repost_ditemid")
//////					entry.SetRepostDItemID(fieldEntry.ValueToLongLong());
//////				else if (fieldEntry.Name() == "repost")
//////					entry.SetRepost(fieldEntry.ValueToBool());
////				else if (fieldEntry.Name() == "allowmask")
////					entry.SetAccess(GetAccessForString(fieldEntry.ValueToString()));

//            }

//            //Dirty hack for sdelano-u-nas.livejournal.com
//            entry.SetEntry(SdelanoUNasEntryPreparing(entry.GetEntry()));
//            entry.SetFullEntry(SdelanoUNasEntryPreparing(entry.GetFullEntry()));

//            return entry;
//        }
//    }

//    LJEntries_t ParseLJEntries(const QDomDocument& document,
//            const QVariantMap& params)
//    {
//        LJEntries_t entries;
//        const auto& firstStructElement = document.elementsByTagName("struct");
//        if (firstStructElement.at(0).isNull())
//            return entries;

//        const auto& members = firstStructElement.at(0).childNodes();
//        for(int i = 0, count = members.count(); i < count; ++i)
//        {
//            const QDomNode& member = members.at(i);
//            if (!member.isElement() ||
//                    member.toElement().tagName() != "member")
//                continue;

//            auto res = ParseMember(member);
//            if (res.Name() == "entries" || res.Name() == "events")
//                for(const auto& entry : res.Value())
//                    entries << CreateLJEntry(entry, params);
//        }

//        return entries;
//    }

//    LJEntry ParseLJEntry(const QDomDocument& document, const QVariantMap& params)
//    {
//        LJEntry entry;
//        const auto& firstStructElement = document.elementsByTagName("struct");
//        if (firstStructElement.at(0).isNull())
//            return entry;

//        const auto& members = firstStructElement.at(0).childNodes();
//        for(int i = 0, count = members.count(); i < count; ++i)
//        {
//            const QDomNode& member = members.at(i);
//            if (!member.isElement() ||
//                    member.toElement().tagName() != "member")
//                continue;

//            auto res = ParseMember(member);
//            if (res.Name() == "events")
//            {
//                for(const auto& entryValue : res.Value())
//                {
//                    entry = CreateLJEntry(entryValue, params);
//                    break;
//                }
//                break;
//            }
//        }
//        return entry;
//    }

}
}
}
