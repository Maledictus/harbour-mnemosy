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

#include <QDomDocument>
#include <QDomElement>
#include <QPair>

#include "src/ljcomment.h"
#include "src/ljevent.h"
#include "src/ljfriend.h"
#include "src/ljfriendsgroup.h"
#include "src/lj-rpc/ljparsertype.h"
#include "src/userprofile.h"

namespace Mnemosy
{
namespace RpcUtils
{
namespace Builder
{
QPair<QDomElement, QDomElement> GetStartPart(const QString& name,
        QDomDocument doc);

QDomElement GetSimpleMemberElement(const QString& nameVal,
        const QString& typeVal, const QString& value, QDomDocument doc);

QPair<QDomElement, QDomElement> GetComplexMemberElement(const QString& nameVal,
        const QString& typeVal, QDomDocument doc);

QDomElement FillServicePart(QDomElement parentElement,
        const QString& login, const QString& password,
        const QString& challenge, QDomDocument document);
}

namespace Parser
{
LJParserType ParseMember(const QDomNode& node);

QVariantList ParseValue(const QDomNode& node);

UserProfile* ParseUserProfile(const QDomDocument& document);

LJEvents_t ParseLJEvents(const QDomDocument& document);

LJEvent ParseLJEvent(const QDomDocument& document);

LJPostComments ParseLJPostComments(const QDomDocument& document);

LJFriendGroups_t ParseFriendGroups(const QDomDocument& document);

LJFriends_t ParseLJFriends(const QDomDocument& document);
}
}
}
