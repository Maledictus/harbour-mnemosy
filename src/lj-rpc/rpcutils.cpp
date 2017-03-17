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

#include <QtDebug>

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
LJFriendGroup CreateGroup(const QVariantList& data)
{
    LJFriendGroup group;
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
        else if (fieldEntry.Name() == "sortorder")
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
                  profile->AddFriendGroup(CreateGroup(friendGroupEntry.toList()));
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

namespace
{
CommentsManagement GetCommentsManagmentFromString(const QString& str)
{
    CommentsManagement cm = CMDefault;
    if (str == "N")
        cm = CMShowComments;
    else if (str == "R")
        cm = CMScreenAnonymouseComments;
    else if (str == "F")
        cm = CMShowFriendsComments;
    else if (str == "L")
        cm = CMScreenNotFromFriendsWithLinks;
    else if (str == "A")
        cm = CMScreenComments;

    return cm;
}

AdultContent GetAdultContentFromString(const QString& str)
{
    if (str == "concepts")
        return ACAdultsFrom14;
    else if (str == "explicit")
        return ACAdultsFrom18;
    else
        return ACWithoutAdultContent;
}

Access GetAccessForString(const QString& access)
{
    if (access == "private")
        return APrivate;
    else if (access == "usemask")
        return ACustom;
    else
        return APublic;
}

LJEntryProperties CreateLJEventPropetries(QStringList& tags,
        const QVariantList& data)
{
    LJEntryProperties props;
    for(const auto& prop : data)
    {
        auto propsFieldEntry = prop.value<LJParserType>();
        if (propsFieldEntry.Name() == "picture_keyword")
        {
            props.SetPostAvatar(propsFieldEntry.ValueToString());
        }
        else if (propsFieldEntry.Name() == "opt_screening")
        {
            props.SetCommentsManagement(GetCommentsManagmentFromString(propsFieldEntry
                    .ValueToString()));
        }
        else if (propsFieldEntry.Name() == "current_music")
        {
            props.SetCurrentMusic(propsFieldEntry.ValueToString());
        }
        else if (propsFieldEntry.Name() == "current_mood")
        {
            props.SetCurrentMood(propsFieldEntry.ValueToString());
        }
        else if (propsFieldEntry.Name() == "current_location")
        {
            props.SetCurrentLocation(propsFieldEntry.ValueToString());
        }
        else if (propsFieldEntry.Name() == "taglist")
        {
            tags = propsFieldEntry.ValueToString().split(", ");
        }
        else if (propsFieldEntry.Name() == "adult_content")
        {
            props.SetAdultContent(GetAdultContentFromString(propsFieldEntry
                    .ValueToString()));
        }
        else if (propsFieldEntry.Name() == "opt_nocomments")
        {
            props.SetCommentsEnabled(!propsFieldEntry.ValueToInt());
        }
        else if (propsFieldEntry.Name() == "repost")
        {
            props.SetIsRepost((propsFieldEntry.ValueToString() == "c"));
        }
        else if (propsFieldEntry.Name() == "repost_url")
        {
            props.SetRepostUrl(propsFieldEntry.ValueToUrl());
        }
        else if (propsFieldEntry.Name() == "reposted_from")
        {
            props.SetRepostedFrom(propsFieldEntry.ValueToString());
        }
        else if (propsFieldEntry.Name() == "repost_author")
        {
            props.SetRepostedAuthor(propsFieldEntry.ValueToString());
        }
        else if (propsFieldEntry.Name() == "repost_subject")
        {
            props.SetRepostSubject(propsFieldEntry.ValueToString());
        }
    }

    return props;
}

QString PrepareSubject(QString subject)
{
    subject.replace("&laquo;", "«");
    subject.replace("&raquo;", "»");
    subject.replace("&nbsp;", " ");
    subject.replace("&nbsp", " ");
    subject.replace("&#8470;", "№");
    subject.replace("&mdash;", "—");
    subject.replace("&thinsp;", " ");
    return subject;
}

LJEvent CreateLJEvent(const QVariant& data, bool shortVariant)
{
    LJEvent event;
    for(const auto& field : data.toList())
    {
        auto fieldEntry = field.value<LJParserType>();
        //Friends entry
        if (fieldEntry.Name() == "userpic")
        {
            for (const auto& userpicField : fieldEntry.Value())
            {
                auto userpicFieldEntry = userpicField.value<LJParserType>();
                if (userpicFieldEntry.Name() == "picid")
                    event.SetUserPicID(userpicFieldEntry.ValueToLongLong());
                else if (userpicFieldEntry.Name() == "userid")
                    event.SetUserID(userpicFieldEntry.ValueToLongLong());
            }
        }
        else if (fieldEntry.Name() == "posterurl")
        {
            event.SetPosterUrl(fieldEntry.ValueToUrl());
        }
        else if (fieldEntry.Name() == "poster_userpic_url")
        {
            event.SetPosterPicUrl(fieldEntry.ValueToUrl());
        }
        else if (fieldEntry.Name() == "postername" ||
                 fieldEntry.Name() == "poster")
        {
            event.SetPosterName(fieldEntry.ValueToString());
        }
        else if (fieldEntry.Name() == "poster_id")
        {
            event.SetPosterID(fieldEntry.ValueToLongLong());
        }
        else if (fieldEntry.Name() == "journalid")
        {
            event.SetJournalID(fieldEntry.ValueToLongLong());
        }
        else if (fieldEntry.Name() == "journaltype")
        {
            switch(fieldEntry.ValueToString()[0].toLatin1())
            {
            case 'P':
                event.SetJournalType(JTPersonal);
                break;
            case 'C':
                event.SetJournalType(JTCommunity);
                break;
            case 'N':
                event.SetJournalType(JTNews);
                break;
            case 'S':
                event.SetJournalType(JTShared);
                break;
            case 'Y':
                event.SetJournalType(JTSyndicated);
                break;
            case 'R':
                event.SetJournalType(JTRenamed);
                break;
            case 'I':
                event.SetJournalType(JTIdentity);
                break;
            default:
                break;
            }
        }
        else if (fieldEntry.Name() == "journalname")
        {
            event.SetJournalName(fieldEntry.ValueToString());
        }
        else if (fieldEntry.Name() == "journalurl")
        {
            event.SetJournalUrl(fieldEntry.ValueToUrl());
        }
        else if (fieldEntry.Name() == "ditemid")
        {
            event.SetDItemID(fieldEntry.ValueToLongLong());
        }
        else if (fieldEntry.Name() == "subject" ||
                fieldEntry.Name() == "subject_raw")
        {
            event.SetSubject(PrepareSubject(fieldEntry.ValueToString()));
        }
        else if (fieldEntry.Name() == "event")
        {
            if (shortVariant)
            {
                event.SetEvent(fieldEntry.ValueToString());
            }
            else
            {
                event.SetFullEvent(fieldEntry.ValueToString());
            }
        }
        else if (fieldEntry.Name() == "event_raw")
        {
            event.SetEvent(fieldEntry.ValueToString());
        }
        else if (fieldEntry.Name() == "logtime")
        {
            auto dt = QDateTime::fromString(fieldEntry.ValueToString(),
                    "yyyy-MM-dd hh:mm:ss");
            if (dt.isValid())
            {
                event.SetLogTime(dt);
            }
            else
            {
                dt = QDateTime::fromTime_t(fieldEntry.ValueToLongLong());
                if (dt.isValid())
                {
                    event.SetPostDate(dt);
                    event.SetLogTime(dt);
                }
            }
        }
        else if (fieldEntry.Name() == "eventtime")
        {
            auto dt = QDateTime::fromString(fieldEntry.ValueToString(),
                    "yyyy-MM-dd hh:mm:ss");
            if (dt.isValid())
            {
                event.SetPostDate(dt);
            }
        }
        else if (fieldEntry.Name() == "props")
        {
            QStringList tags;
            event.SetProperties(CreateLJEventPropetries(tags, fieldEntry.Value()));
            event.SetTags(tags);
        }
        else if (fieldEntry.Name() == "reply_count")
        {
            event.SetReplyCount(fieldEntry.ValueToInt());
        }
        else if (fieldEntry.Name() == "security")
        {
            event.SetAccess(GetAccessForString(fieldEntry.ValueToString()));
        }
        else if (fieldEntry.Name() == "itemid")
        {
            event.SetItemID(fieldEntry.ValueToLongLong());
        }
        else if (fieldEntry.Name() == "can_comment")
        {
            event.SetCanComment(fieldEntry.ValueToInt());
        }
        else if (fieldEntry.Name() == "url")
        {
            event.SetUrl(QUrl(fieldEntry.ValueToUrl()));
        }
        else if (fieldEntry.Name() == "allowmask")
        {
            event.SetAllowMask(fieldEntry.ValueToInt());
        }
        else if (fieldEntry.Name() == "anum")
        {
            event.SetAnum(fieldEntry.ValueToInt());
        }
        else if (fieldEntry.Name() == "repost_ditemid")
        {
            event.SetRepostDItemId(fieldEntry.ValueToLongLong());
        }
        else if (fieldEntry.Name() == "repost")
        {
            event.SetIsRepost(fieldEntry.ValueToBool());
        }
        else if (fieldEntry.Name() == "repostername")
        {
            event.SetReposterName(fieldEntry.ValueToString());
        }
        else if (fieldEntry.Name() == "ownerid")
        {
            event.SetOwnerId(fieldEntry.ValueToLongLong());
        }
        else if (fieldEntry.Name() == "original_entry_url")
        {
            event.SetOriginalEntryUrl(fieldEntry.ValueToUrl());
        }


    }
    return event;
}

LJCommentProperties CreateLJCommentPropetries(const QVariantList& data)
{
    LJCommentProperties props;
    for(const auto& prop : data)
    {
        auto propsFieldEntry = prop.value<LJParserType>();
        if (propsFieldEntry.Name() == "edit_time")
        {
            props.SetEditTime(QDateTime::fromTime_t(propsFieldEntry.ValueToLongLong()));
        }
        else if (propsFieldEntry.Name() == "deleted_posterg")
        {
            props.SetDeletedPoster(propsFieldEntry.ValueToBool());
        }
        else if (propsFieldEntry.Name() == "picture_keyword")
        {
            props.SetPictureKeyword(propsFieldEntry.ValueToString());
        }
    }

    return props;
}

LJComment CreateLJComment(const QVariant& data)
{
    LJComment comment;
    for(const auto& field : data.toList())
    {
        auto fieldEntry = field.value<LJParserType>();
        if (fieldEntry.Name() == "userpic")
        {
            comment.SetUserPicUrl(fieldEntry.ValueToUrl());
        }
        else if (fieldEntry.Name() == "privileges")
        {
            LJComment::Privileges privs;
            for (const auto& priv : fieldEntry.Value())
            {
                auto privFieldEntry = priv.value<LJParserType>();
                if (privFieldEntry.Name() == "reply" &&
                        privFieldEntry.ValueToInt())
                {
                    privs |= LJComment::PReply;
                }
                else if (privFieldEntry.Name() == "edit" &&
                         privFieldEntry.ValueToInt())
                {
                    privs |= LJComment::PEdit;
                }
                else if (privFieldEntry.Name() == "delete" &&
                         privFieldEntry.ValueToInt())
                {
                    privs |= LJComment::PDelete;
                }
                else if (privFieldEntry.Name() == "freeze" &&
                         privFieldEntry.ValueToInt())
                {
                    privs |= LJComment::PFreeze;
                }
                else if (privFieldEntry.Name() == "unfreeze" &&
                         privFieldEntry.ValueToInt())
                {
                    privs |= LJComment::PUnfreeze;
                }
                else if (privFieldEntry.Name() == "spam" &&
                         privFieldEntry.ValueToInt())
                {
                    privs |= LJComment::PSpam;
                }
                else if (privFieldEntry.Name() == "unspam" &&
                         privFieldEntry.ValueToInt())
                {
                    privs |= LJComment::PUnspam;
                }
                else if (privFieldEntry.Name() == "screen" &&
                         privFieldEntry.ValueToInt())
                {
                    privs |= LJComment::PScreen;
                }
                else if (privFieldEntry.Name() == "unscreen" &&
                         privFieldEntry.ValueToInt())
                {
                    privs |= LJComment::PUnscreen;
                }
                else if (privFieldEntry.Name() == "best" &&
                         privFieldEntry.ValueToInt())
                {
                    privs |= LJComment::PBest;
                }
                else if (privFieldEntry.Name() == "ban" &&
                         privFieldEntry.ValueToInt())
                {
                    privs |= LJComment::PBan;
                }
            }
            comment.SetPrivileges(privs);
        }
        else if (fieldEntry.Name() == "children")
        {
            LJComments_t children;
            for(const auto& entry : fieldEntry.Value())
            {
                children << CreateLJComment(entry);
            }
            comment.SetChildren(children);
        }
        else if (fieldEntry.Name() == "posterid")
        {
            comment.SetPosterID(fieldEntry.ValueToLongLong());
        }
        else if (fieldEntry.Name() == "state")
        {
            comment.SetState(fieldEntry.ValueToString());
        }
        else if (fieldEntry.Name() == "subject")
        {
            comment.SetSubject(fieldEntry.ValueToString());
        }
        else if (fieldEntry.Name() == "body")
        {
            comment.SetBody(fieldEntry.ValueToString());
        }
        else if (fieldEntry.Name() == "poster_userpic_url")
        {
            comment.SetPosterPicUrl(fieldEntry.ValueToUrl());
        }
        else if (fieldEntry.Name() == "dtalkid")
        {
            comment.SetDTalkID(fieldEntry.ValueToLongLong());
        }
        else if (fieldEntry.Name() == "postername")
        {
            comment.SetPosterName(fieldEntry.ValueToString());
        }
        else if (fieldEntry.Name() == "datepostunix")
        {
            comment.SetDatePostUnix(QDateTime::fromTime_t(fieldEntry.ValueToLongLong()));
        }
        else if (fieldEntry.Name() == "parentdtalkid")
        {
            comment.SetParentTalkID(fieldEntry.ValueToLongLong());
        }
        else if (fieldEntry.Name() == "level")
        {
            comment.SetLevel(fieldEntry.ValueToInt());
        }
        else if (fieldEntry.Name() == "is_show")
        {
            comment.SetShown(fieldEntry.ValueToBool());
        }
        else if (fieldEntry.Name() == "props")
        {
            comment.SetProperties(CreateLJCommentPropetries(fieldEntry.Value()));
        }
        else if (fieldEntry.Name() == "is_loaded")
        {
            comment.SetLoaded(fieldEntry.ValueToBool());
        }
        else if (fieldEntry.Name() == "thread_count")
        {
            comment.SetThreadCount(fieldEntry.ValueToLongLong());
        }
    }

    if (comment.GetState() == "D")
    {
        comment.SetBody("<i>Deleted</i>");
    }

    return comment;
}

LJFriendGroup CreateLJFriendGroup(const QVariantList& data)
{
    LJFriendGroup group;
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
        else if (fieldEntry.Name() == "sortorder")
        {
            group.SetSortOrder(fieldEntry.ValueToInt());
        }
        else if (fieldEntry.Name() == "public")
        {
            group.SetPublic(fieldEntry.ValueToInt());
        }
    }

    return group;
}

void CreateLJFriend(const QString& parentKey, const QVariantList& data,
        QHash<QString, LJFriend>& frHash)
{
    for (const auto& friendEntry : data)
    {
        LJFriend fr;
        bool isCommunity = false;
        bool personal = false;

        for (const auto& field : friendEntry.toList())
        {
            auto fieldEntry = field.value<LJParserType>();
            if (fieldEntry.Name() == "defaultpicurl")
            {
                // Ignore avatar from LJ server due to invalid url
            }
            else if (fieldEntry.Name() == "groupmask")
            {
                fr.SetGroupMask(fieldEntry.ValueToInt());
            }
            else if (fieldEntry.Name() == "bgcolor")
            {
                fr.SetBGColor(fieldEntry.ValueToString());
            }
            else if (fieldEntry.Name() == "fgcolor")
            {
                fr.SetFGColor(fieldEntry.ValueToString());
            }
            else if (fieldEntry.Name() == "fullname")
            {
                fr.SetFullName(fieldEntry.ValueToString());
            }
            else if (fieldEntry.Name() == "username")
            {
                fr.SetUserName(fieldEntry.ValueToString());
            }
            else if (fieldEntry.Name() == "type")
            {
                isCommunity = fieldEntry.ValueToString() == "community";
            }
            else if (fieldEntry.Name() == "journaltype")
            {
                isCommunity =(fieldEntry.ValueToString() == "C");
                personal =(fieldEntry.ValueToString() == "P");
            }
            else if (fieldEntry.Name() == "birthday")
            {
                fr.SetBirthday(fieldEntry.ValueToString());
            }

            if (parentKey == "friends" || parentKey == "added")
            {
                fr.SetMyFriend(true);
            }

            if (parentKey == "friendofs")
            {
                fr.SetFriendOf(true);
            }
        }

        if (!isCommunity || personal)
        {
            if (parentKey == "friendofs" && frHash.contains(fr.GetUserName()))
            {
                frHash[fr.GetUserName()].SetFriendOf(true);
            }
            else if ((parentKey == "friends" || parentKey == "added") &&
                    frHash.contains(fr.GetUserName()))
            {
                frHash[fr.GetUserName()].SetMyFriend(true);
            }
            else
            {
                frHash[fr.GetUserName()] = fr;
            }
        }
    }
}
}

LJEvents_t ParseLJEvents(const QDomDocument& document)
{
    LJEvents_t events;
    const auto& firstStructElement = document.elementsByTagName("struct");
    if (firstStructElement.at(0).isNull())
    {
        return events;
    }

    const auto& members = firstStructElement.at(0).childNodes();
    for(int i = 0, count = members.count(); i < count; ++i)
    {
        const QDomNode& member = members.at(i);
        if (!member.isElement() ||
                member.toElement().tagName() != "member")
        {
            continue;
        }

        auto res = ParseMember(member);
        if (res.Name() == "entries" || res.Name() == "events")
        {
            for(const auto& entry : res.Value())
            {
                events << CreateLJEvent(entry, true);
            }
        }
    }

    return events;
}

LJEvent ParseLJEvent(const QDomDocument& document)
{
    LJEvent event;
    const auto& firstStructElement = document.elementsByTagName("struct");
    if (firstStructElement.at(0).isNull())
    {
        return event;
    }

    const auto& members = firstStructElement.at(0).childNodes();
    for(int i = 0, count = members.count(); i < count; ++i)
    {
        const QDomNode& member = members.at(i);
        if (!member.isElement() ||
                member.toElement().tagName() != "member")
        {
            continue;
        }

        auto res = ParseMember(member);
        if (res.Name() == "events")
        {
            for(const auto& entryValue : res.Value())
            {
                event = CreateLJEvent(entryValue, false);
                break;
            }
        }
    }
    return event;
}

LJPostComments ParseLJPostComments(const QDomDocument& document)
{
    LJPostComments postComments;
    const auto& firstStructElement = document.elementsByTagName("struct");
    if (firstStructElement.at(0).isNull())
    {
        return postComments;
    }

    const auto& members = firstStructElement.at(0).childNodes();
    for(int i = 0, count = members.count(); i < count; ++i)
    {
        const QDomNode& member = members.at(i);
        if (!member.isElement() ||
                member.toElement().tagName() != "member")
        {
            continue;
        }

        auto res = ParseMember(member);
        if (res.Name() == "topitem_first")
        {
            postComments.m_TopItemFirst = res.ValueToLongLong();
        }
        else if (res.Name() == "topitems")
        {
            postComments.m_TopItems = res.ValueToLongLong();
        }
        else if (res.Name() == "page")
        {
            postComments.m_Page = res.ValueToLongLong();
        }
        else if (res.Name() == "ditemid")
        {
            postComments.m_DItemId = res.ValueToLongLong();
        }
        else if (res.Name() == "page_size")
        {
            postComments.m_PageSize = res.ValueToLongLong();
        }
        else if (res.Name() == "lastsync")
        {
            postComments.m_LastSync = QDateTime::fromTime_t(res.ValueToLongLong());
        }
        else if (res.Name() == "best_dtalkid")
        {
            postComments.m_BestDTalkId = res.ValueToLongLong();
        }
        else if (res.Name() == "pages")
        {
            postComments.m_Pages = res.ValueToLongLong();
        }
        if (res.Name() == "comments")
        {
            for(const auto& entry : res.Value())
            {
                postComments.m_Comments << CreateLJComment(entry);
            }
        }
    }
    return postComments;
}

LJFriendGroups_t ParseFriendGroups(const QDomDocument& document)
{
    LJFriendGroups_t groups;
    const auto& firstStructElement = document.elementsByTagName("struct");
    if (firstStructElement.at(0).isNull())
    {
        return groups;
    }

    const auto& members = firstStructElement.at(0).childNodes();
    for (int i = 0, count = members.count(); i < count; ++i)
    {
        const QDomNode& member = members.at(i);
        if (!member.isElement() ||
                member.toElement().tagName() != "member")
        {
            continue;
        }

        auto res = ParseMember(member);
        if (res.Name() == "friendgroups")
        {
            for (const auto& group : res.Value())
            {
                groups << CreateLJFriendGroup(group.toList());
            }
        }
    }

    return groups;
}

LJFriends_t ParseLJFriends(const QDomDocument& document)
{
    LJFriends_t friends;
    const auto& firstStructElement = document.elementsByTagName("struct");
    if (firstStructElement.at(0).isNull())
    {
        return friends;
    }

    const auto& members = firstStructElement.at(0).childNodes();
    QHash<QString, LJFriend> frHash;
    for (int i = 0, count = members.count(); i < count; ++i)
    {
        const QDomNode& member = members.at(i);
        if (!member.isElement() ||
                member.toElement().tagName() != "member")
        {
            continue;
        }

        auto res = ParseMember(member);
        if (res.Name () == "friends" ||
                res.Name () == "added" ||
                res.Name () == "friendofs")
        {
            CreateLJFriend(res.Name (), res.Value (), frHash);
        }
    }

    return frHash.values();
}

}
}
}
