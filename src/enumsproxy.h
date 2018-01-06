/*
The MIT License (MIT)

Copyright (c) 2016-2018 Oleg Linkin <maledictusdemagog@gmail.com>

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

#include <QObject>
#include "ljcomment.h"
#include "ljeventproperties.h"
#include "ljmessage.h"
#include "models/friendssortfilterproxymodel.h"

namespace Mnemosy
{
enum ModelType
{
    MTUnknown = 0,
    MTFeed,
    MTUserBlog,
    MTMyBlog
};

enum ErrorType
{
    ETGeneral = 0,
    ETLiveJournal
};

enum DeleteCommentType
{
    DCTComment = 1,
    DCTThread = 2,
    DCTAllComments = 4
};

class EnumsProxy : public QObject
{
    Q_OBJECT

    Q_ENUMS(JournalTypeProxy)
    Q_ENUMS(ModelTypeProxy)
    Q_ENUMS(CommentPrivilegesProxy)
    Q_ENUMS(ErrorTypeProxy)
    Q_ENUMS(FriendsFilterProxy)
    Q_ENUMS(FriendsSortProxy)
    Q_ENUMS(DeleteCommentTypeProxy)
    Q_ENUMS(MessageDirectionProxy)
    Q_ENUMS(MessageStateProxy)
    Q_ENUMS(MessageActionProxy)
    Q_ENUMS(MessageTypeProxy)
    Q_ENUMS(EventAccessProxy)
    Q_ENUMS(EventAdultProxy)
    Q_ENUMS(EventScreeningProxy)

public:

    enum JournalTypeProxy
    {
        PersonalJournal = JTPersonal,
        CommunityJournal = JTCommunity,
        NewsJournal = JTNews,
        SharedJournal = JTShared,
        SyndicatedJournal = JTSyndicated,
        RenamedJournal = JTRenamed,
        IdentityJournal = JTIdentity
    };

    enum ModelTypeProxy
    {
        FeedModel = MTFeed,
        UserModel = MTUserBlog,
        MyModel = MTMyBlog
    };

    enum CommentPrivilegesProxy
    {
        Freeze = LJComment::PFreeze,
        Unbest = LJComment::PUnbest,
        Ban = LJComment::PBan,
        Edit = LJComment::PEdit,
        Unspam = LJComment::PUnspam,
        Unfreeze = LJComment::PUnfreeze,
        Reply = LJComment::PReply,
        Spam = LJComment::PSpam,
        Screen = LJComment::PScreen,
        Delete = LJComment::PDelete,
        Unscreen = LJComment::PUnscreen,
        Best = LJComment::PBest
    };

    enum ErrorTypeProxy
    {
        GeneralError = ETGeneral,
        LiveJournalError = ETLiveJournal
    };

    enum FriendsFilterProxy
    {
        TextFilter = FriendsSortFilterProxyModel::FMText,
        GroupFilter = FriendsSortFilterProxyModel::FMGroup
    };

    enum FriendsSortProxy
    {
        TextSort = FriendsSortFilterProxyModel::SMText,
        StatusSort = FriendsSortFilterProxyModel::SMStatus
    };

    enum DeleteCommentTypeProxy
    {
        DeleteComment = DCTComment,
        DeleteThread = DCTThread,
        DeleteAllComment = DCTAllComments
    };

    enum MessageDirectionProxy
    {
        Inbox = LJMessage::DIn,
        Outbox = LJMessage::DOut
    };

    enum MessageStateProxy
    {
        Read = LJMessage::SRead,
        Unread = LJMessage::SUnread
    };

    enum MessageActionProxy
    {
        Deleted = LJMessage::ADeleted,
        CommentDeleted = LJMessage::ACommentDeleted,
        Edited = LJMessage::AEdited,
        New = LJMessage::ANew
    };

    enum MessageTypeProxy
    {
        BeFriended = LJMessage::MTBeFriended,
        Birthday = LJMessage::MTBirthday,
        CommunityInvite = LJMessage::MTCommunityInvite,
        CommunityJoinApprove = LJMessage::MTCommunityJoinApprove,
        CommunityJoinReject = LJMessage::MTCommunityJoinReject,
        CommunityJoinRequest = LJMessage::MTCommunityJoinRequest,
        Defriended = LJMessage::MTDefriended,
        InvitedFriendJoins = LJMessage::MTInvitedFriendJoins,
        JournalNewComment = LJMessage::MTJournalNewComment,
        JournalNewEntry = LJMessage::MTJournalNewEntry,
        NewUserpic = LJMessage::MTNewUserpic,
        NewVGift = LJMessage::MTNewVGift,
        OfficialPost = LJMessage::MTOfficialPost,
        PermSale = LJMessage::MTPermSale,
        PollVote = LJMessage::MTPollVote,
        SupOfficialPost = LJMessage::MTSupOfficialPost,
        UserExpunged = LJMessage::MTUserExpunged,
        UserMessageRecvd = LJMessage::MTUserMessageRecvd,
        UserMessageSent = LJMessage::MTUserMessageSent,
        UserNewComment = LJMessage::MTUserNewComment,
        UserNewEntry = LJMessage::MTUserNewEntry,
        CommentReply = LJMessage::MTCommentReply
    };

    enum EventAccessProxy
    {
        PublicAccess = APublic,
        FriendsOnlyAccess = AFriendsOnly,
        PrivateAccess = APrivate,
        CustomAccess = ACustom
    };

    enum EventAdultProxy
    {
        WithoutAdult = ACWithoutAdultContent,
        AdutlFrom14 = ACAdultsFrom14,
        AdultFrom18 = ACAdultsFrom18
    };

    enum EventScreeningProxy
    {
        NoScreening = CMShowComments,
        ScreenNoFriends = CMShowFriendsComments,
        ScreenAll = CMScreenComments,
        ScreenAnonymous = CMScreenAnonymouseComments,
        ScreenWithLinks = CMScreenNotFromFriendsWithLinks
    };
};
}
