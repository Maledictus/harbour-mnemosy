﻿/*
The MIT License(MIT)

Copyright (c) 2016-2018 Oleg Linkin <maledictusdemagog@gmail.com>

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

#include "ljxmlrpc.h"
#include <QDomElement>
#include <QDomNode>
#include <QDomProcessingInstruction>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QXmlQuery>

#include "src/lj-rpc/rpcutils.h"
#include "src/settings/accountsettings.h"
#include "src/utils.h"

namespace Mnemosy
{
LJXmlRPC::LJXmlRPC(QObject *parent)
: QObject(parent)
, m_NAM(new QNetworkAccessManager(this))
{
}

void LJXmlRPC::AbortRequest()
{
    if (m_CurrentReply)
    {
        m_CurrentReply->abort();
    }
}

void LJXmlRPC::Login(const QString& login, const QString& password)
{
    auto guard = MakeRunnerGuard();
    m_ApiCallQueue << [this](const QString&){ GetChallenge(); };
    m_ApiCallQueue << [login, password, this](const QString& challenge)
        { Login(login, password, challenge); };
}

void LJXmlRPC::GetFriendsPage(const QDateTime& before, int groupMask)
{
    auto guard = MakeRunnerGuard();
    m_ApiCallQueue << [this](const QString&){ GetChallenge(); };
    m_ApiCallQueue << [before, groupMask, this](const QString& challenge)
       { GetFriendsPage(before, groupMask, challenge); };
}

void LJXmlRPC::GetEvent(quint64 dItemId, const QString& journalName, ModelType mt)
{
    auto guard = MakeRunnerGuard();
    m_ApiCallQueue << [this](const QString&){ GetChallenge(); };
    m_ApiCallQueue << [dItemId, journalName, mt, this](const QString& challenge)
        {
            GetEvents({ { "ditemid", "int", QString::number(dItemId) } },
                    journalName, SelectType::One, mt, challenge);
        };
}

void LJXmlRPC::AddComment(const QString& journalName, quint64 parentTalkId,
        quint64 dItemId, const QString& subject, const QString& body, const QString& avatarId)
{
    auto guard = MakeRunnerGuard();
    m_ApiCallQueue << [this](const QString&){ GetChallenge(); };
    m_ApiCallQueue << [journalName, parentTalkId, dItemId, subject, body, avatarId, this]
            (const QString& challenge)
        {
            AddComment(journalName, parentTalkId, dItemId, subject, body, avatarId, challenge);
        };
}

void LJXmlRPC::EditComment(const QString& journalName, quint64 dTalkId,
        const QString& subject, const QString& body)
{
    auto guard = MakeRunnerGuard();
    m_ApiCallQueue << [this](const QString&){ GetChallenge(); };
    m_ApiCallQueue << [journalName, dTalkId, subject, body, this]
            (const QString& challenge)
        {
            EditComment(journalName, dTalkId, subject, body, challenge);
        };
}

void LJXmlRPC::DeleteComment(const QString& journalName, quint64 dTalkId, int deleteMask)
{
    auto guard = MakeRunnerGuard();
    m_ApiCallQueue << [this](const QString&){ GetChallenge(); };
    m_ApiCallQueue << [journalName, dTalkId, deleteMask, this]
            (const QString& challenge)
        {
            DeleteComment(journalName, dTalkId, deleteMask, challenge);
        };
}

void LJXmlRPC::GetComments(quint64 dItemId, const QString& journal, int page,
        quint64 dTalkId)
{
    auto guard = MakeRunnerGuard();
    m_ApiCallQueue << [this](const QString&){ GetChallenge(); };
    m_ApiCallQueue << [dItemId, journal, page,dTalkId, this](const QString& challenge)
            { GetComments(dItemId, journal, page, dTalkId, challenge); };
}

void LJXmlRPC::GetUpdatedComment(quint64 dItemId, const QString& journal, quint64 dTalkId)
{
    auto guard = MakeRunnerGuard();
    m_ApiCallQueue << [this](const QString&){ GetChallenge(); };
    m_ApiCallQueue << [dItemId, journal, dTalkId, this](const QString& challenge)
            { GetUpdatedComment(dItemId, journal, dTalkId, challenge); };
}

void LJXmlRPC::UpdateComment(quint64 dItemId, const QString& journalName, quint64 dTalkId,
        LJComment::Privilege action)
{
    auto guard = MakeRunnerGuard();
    m_ApiCallQueue << [this](const QString&){ GetChallenge(); };
    m_ApiCallQueue << [dItemId, journalName, dTalkId, action, this](const QString& challenge)
        { UpdateComment(dItemId, journalName, dTalkId, action, challenge); };
}

void LJXmlRPC::GetFriendGroups()
{
    auto guard = MakeRunnerGuard();
    m_ApiCallQueue << [this](const QString&){ GetChallenge(); };
    m_ApiCallQueue << [this](const QString& challenge)
        { GetFriendGroups(challenge); };
}

void LJXmlRPC::AddFriendGroup(const QString& name, bool isPrivate, int id)
{
    auto guard = MakeRunnerGuard();
    m_ApiCallQueue << [this](const QString&){ GetChallenge(); };
    m_ApiCallQueue << [name, isPrivate, id, this](const QString& challenge)
        { AddFriendGroup(name, isPrivate, id, challenge); };
}

void LJXmlRPC::DeleteFriendGroup(quint64 groupId)
{
    auto guard = MakeRunnerGuard();
    m_ApiCallQueue << [this](const QString&){ GetChallenge(); };
    m_ApiCallQueue << [groupId, this](const QString& challenge)
        { DeleteFriendGroup(groupId, challenge); };
}

void LJXmlRPC::LoadUserJournal(const QString& journalName,
        const QDateTime& before, ModelType mt)
{
    auto guard = MakeRunnerGuard ();
    m_ApiCallQueue << [this] (const QString&) { GetChallenge(); };
    m_ApiCallQueue << [journalName, before, mt, this] (const QString& challenge)
        { LoadUserJournal(journalName, before, mt, challenge); };
}

void LJXmlRPC::GetFriends()
{
    auto guard = MakeRunnerGuard ();
    m_ApiCallQueue << [this] (const QString&) { GetChallenge(); };
    m_ApiCallQueue << [this] (const QString& challenge)
        { GetFriends(challenge); };
}

void LJXmlRPC::AddFriend(const QString& name, uint groupMask)
{
    auto guard = MakeRunnerGuard ();
    m_ApiCallQueue << [this] (const QString&) { GetChallenge(); };
    m_ApiCallQueue << [name, groupMask, this]
            (const QString& challenge)
        { AddFriend(name, groupMask, challenge); };
}

void LJXmlRPC::EditFriend(const QString& name, uint groupMask)
{
    auto guard = MakeRunnerGuard ();
    m_ApiCallQueue << [this] (const QString&) { GetChallenge(); };
    m_ApiCallQueue << [name, groupMask, this]
            (const QString& challenge)
        { EditFriend(name, groupMask, challenge); };
}

void LJXmlRPC::DeleteFriend(const QString& name)
{
    auto guard = MakeRunnerGuard ();
    m_ApiCallQueue << [this] (const QString&) { GetChallenge(); };
    m_ApiCallQueue << [name, this] (const QString& challenge)
        { DeleteFriend(name, challenge); };
}

void LJXmlRPC::GetMessages(quint64 lastSyncTime, const LJMessages_t& msgs)
{
    auto guard = MakeRunnerGuard ();
    m_ApiCallQueue << [this] (const QString&) { GetChallenge(); };
    m_ApiCallQueue << [lastSyncTime, msgs, this] (const QString& challenge)
        { GetMessages(lastSyncTime, challenge, msgs); };
}

void LJXmlRPC::GetNotifications(quint64 lastSyncTime, const LJMessages_t& msgs)
{
    auto guard = MakeRunnerGuard ();
    m_ApiCallQueue << [this] (const QString&) { GetChallenge(); };
    m_ApiCallQueue << [lastSyncTime, msgs, this] (const QString& challenge)
        { GetNotifications(lastSyncTime, challenge, msgs); };
}

void LJXmlRPC::MarkAsRead(const QList<quint64>& qids)
{
    auto guard = MakeRunnerGuard ();
    m_ApiCallQueue << [this] (const QString&) { GetChallenge(); };
    m_ApiCallQueue << [qids, this] (const QString& challenge)
        { MarkAsRead(qids, challenge); };
}

void LJXmlRPC::SendMessage(const QString& to, const QString& subject, const QString& body,
        const qint64 parentMessageId)
{
    auto guard = MakeRunnerGuard ();
    m_ApiCallQueue << [this] (const QString&) { GetChallenge(); };
    m_ApiCallQueue << [to, subject, body, parentMessageId, this] (const QString& challenge)
        { SendMessage(to, subject, body, parentMessageId, challenge); };
}

void LJXmlRPC::PostEvent(const LJEvent& event)
{
    auto guard = MakeRunnerGuard ();
    m_ApiCallQueue << [this] (const QString&) { GetChallenge(); };
    m_ApiCallQueue << [event, this] (const QString& challenge)
        { PostEvent(event, challenge); };
}

void LJXmlRPC::EditEvent(const LJEvent& event)
{
    auto guard = MakeRunnerGuard ();
    m_ApiCallQueue << [this] (const QString&) { GetChallenge(); };
    m_ApiCallQueue << [event, this] (const QString& challenge)
        { EditEvent(event, challenge); };
}

void LJXmlRPC::DeleteEvent(const quint64 itemId, const QString& journal)
{
    auto guard = MakeRunnerGuard ();
    m_ApiCallQueue << [this] (const QString&) { GetChallenge(); };
    m_ApiCallQueue << [itemId, journal, this] (const QString& challenge)
        { DeleteEvent(itemId, journal, challenge); };
}

std::shared_ptr<void> LJXmlRPC::MakeRunnerGuard()
{
    const bool shouldRun = m_ApiCallQueue.isEmpty();
    return std::shared_ptr<void>(nullptr, [this, shouldRun](void*)
        {
            if (shouldRun)
            {
                m_ApiCallQueue.dequeue()(QString());
            }
        });
}

QDomDocument LJXmlRPC::PreparsingReply(QObject* sender, bool& ok)
{
    QDomDocument doc;
    auto reply = qobject_cast<QNetworkReply*> (sender);
    if (!reply)
    {
        qDebug() << "Invalid reply";
        emit error(tr("Invalid server reply"));
        ok = false;
        return doc;
    }
    reply->deleteLater();
    m_CurrentReply.clear();

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << Q_FUNC_INFO << "There is network error: "
                << reply->error() << reply->errorString();
        emit error(tr("Network error %1: %2").arg(reply->error()).arg(reply->errorString()),
                reply->error(), ETGeneral);
        ok = false;
        return doc;
    }
    QByteArray data = reply->readAll();
    ok = false;
    doc = ParseDocument(data, ok);
    if (!ok)
    {
        qDebug() << "Unable to generate xml from reply";
        emit error(tr("Reply data is corrupted"));
        return doc;
    }

    ok = true;
    return doc;
}

QDomDocument LJXmlRPC::ParseDocument(const QByteArray& data, bool& ok)
{
    QDomDocument document;
    QString errorMsg;
    int errorLine = -1;
    int errorColumn = -1;
    if (!document.setContent(data, &errorMsg, &errorLine, &errorColumn))
    {
        qDebug() << Q_FUNC_INFO
                << errorMsg
                << "in line:"
                << errorLine
                << "column:"
                << errorColumn;
        ok = false;
    }
    else
    {
        ok = true;
    }
    return document;
}

QPair<int, QString> LJXmlRPC::CheckOnLJErrors(const QDomDocument& doc)
{
    QXmlQuery query;
    query.setFocus(doc.toByteArray());
    QString errorCode;
    query.setQuery("/methodResponse/fault/value/struct/\
            member[name='faultCode']/value/int/text()");
    if (!query.evaluateTo(&errorCode))
    {
        errorCode = QString();
    }

    QString errorString;
    query.setQuery("/methodResponse/fault/value/struct/\
            member[name='faultString']/value/string/text()");
    if (!query.evaluateTo(&errorString))
    {
        errorString = QString();
    }

    const int error = errorCode.toInt();
    if (error)
    {
        if (error == 100 || error == 101 || error == 402)
        {
            emit logged(false, QString(), QString());
        }
    }

    return qMakePair(error, errorString);
}


namespace
{
QNetworkRequest CreateNetworkRequest()
{
    QNetworkRequest request;
    request.setUrl(QUrl("https://www.livejournal.com/interface/xmlrpc"));
    request.setRawHeader("User-Agent",
            QString("Mnemosy " + QString(APP_VERSION)).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml");
    request.setHeader(QNetworkRequest::SetCookieHeader, QVariant());
    request.setHeader(QNetworkRequest::CookieHeader, QVariant());
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Accept-Encoding", "*");
    request.setRawHeader("Accept-Language", "*");

    return request;
}

QString GetPassword(const QString& password, const QString& challenge)
{
    const QByteArray passwordHash = QCryptographicHash::hash(password.toUtf8(),
            QCryptographicHash::Md5).toHex();
    return QCryptographicHash::hash((challenge + passwordHash).toUtf8(),
            QCryptographicHash::Md5).toHex();
}
}

void LJXmlRPC::GetChallenge()
{
    QDomDocument document;
    QDomProcessingInstruction header = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(header);
    QDomElement methodCall = document.createElement("methodCall");
    document.appendChild(methodCall);
    QDomElement methodName = document.createElement("methodName");
    methodCall.appendChild(methodName);
    QDomText methodNameText = document.createTextNode("LJ.XMLRPC.getchallenge");
    methodName.appendChild(methodNameText);

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleGetChallenge);
}

void LJXmlRPC::Login(const QString& login, const QString& password,
        const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction header = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(header);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.login", document);
    document.appendChild(result.first);
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("clientversion",
            "string", "Sailfish-Mnemosy: " + QString(APP_VERSION),
            document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("getmoods",
            "int", "1000", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("getmenus",
            "int", "0", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("getpickws",
            "int", "1", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("getpickwurls",
            "int", "1", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("getcaps",
            "int", "1", document));

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            [this, login, password]()
            {
                handleLogin(login, password);
            });
}

void LJXmlRPC::GetFriendsPage(const QDateTime& before, int groupMask,
        const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction header = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(header);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.getfriendspage",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart (result.second, login,
            GetPassword(password, challenge), challenge, document);
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("before",
            "string", QString::number((before.toMSecsSinceEpoch() / 1000) - 1), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("itemshow",
            "int", QString::number(ItemShow), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("parseljtags",
            "boolean", "true", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("trim_widgets",
            "int", QString::number(LJXmlRPC::TrimWidgets), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("widgets_img_length",
            "int", QString::number(LJXmlRPC::WidgetsImgLength), document));
    if (groupMask > 0)
    {
        element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("groupmask",
                "int", QString::number(groupMask), document));
    }
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("asxml",
            "boolean", "true", document));

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    m_CurrentReply = reply;
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleGetFriendsPage);
}

namespace
{
    QString ToString(SelectType st)
    {
        switch (st)
        {
        case SelectType::One:
            return "one";
        case SelectType::Day:
            return "day";
        case SelectType::LastN:
            return "lastn";
        case SelectType::SyncItems:
            return "syncitems";
        case SelectType::Multiple:
            return "multiple";
        case SelectType::Before:
            return "before";
        };
        return "";
    }

    QString ToString(LJComment::Privilege action)
    {
        switch (action)
        {
        case LJComment::PScreen:
            return "screen";
        case LJComment::PUnscreen:
            return "unscreen";
        case LJComment::PFreeze:
            return "freeze";
        case LJComment::PUnfreeze:
            return "unfreeze";
        case LJComment::PSpam:
            return "spam";
        case LJComment::PUnspam:
            return "unspam";
        case LJComment::PBest:
            return "best";
        case LJComment::PUnbest:
            return "unbest";
        default:
            return "unscreen";
        }
    }
}

QDomDocument LJXmlRPC::GenerateGetEventsRequest(const QList<GetEventsInfo>& infos,
        const QString& journalName, SelectType st, const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction header = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(header);
    auto result = RpcUtils::Builder::GetStartPart ("LJ.XMLRPC.getevents",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart (result.second, login,
            GetPassword(password, challenge), challenge, document);
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("selecttype",
            "string", ToString(st), document));
    for (const auto& info : infos)
    {
        element.appendChild(RpcUtils::Builder::GetSimpleMemberElement(info.m_Key,
                info.m_Type, info.m_Value, document));
    }
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("lineendings",
            "string", "unix", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("usejournal",
            "string", journalName, document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("parseljtags",
            "boolean", "true", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("asxml",
            "boolean", "true", document));

    return document;
}

QDomDocument LJXmlRPC::GenerateEditFriendsRequest(const QString& name,
        uint groupMask, const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.editfriends",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);
    auto array = RpcUtils::Builder::GetComplexMemberElement("add", "array",
            document);
    element.appendChild(array.first);
    auto structField = document.createElement("struct");
    array.second.appendChild(structField);
    structField.appendChild(RpcUtils::Builder::GetSimpleMemberElement("username",
            "string", name, document));
    structField.appendChild(RpcUtils::Builder::GetSimpleMemberElement("groupmask",
            "int", QString::number(groupMask), document));

    return document;
}

QDomDocument LJXmlRPC::GenerateGetInboxRequest(const quint64 lastSync,
        const QList<int>& types, const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.getinbox",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("lastsync",
            "string", QString::number(lastSync), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("extended",
            "boolean", "true", document));
    auto array = RpcUtils::Builder::GetComplexMemberElement("gettype", "array",
            document);
    element.appendChild(array.first);
    for (const int type : types)
    {
        QDomElement valueField = document.createElement("value");
        array.second.appendChild(valueField);
        QDomElement valueType = document.createElement("int");
        valueField.appendChild(valueType);
        QDomText text = document.createTextNode(QString::number(type));
        valueType.appendChild(text);
    }

    return document;
}

void LJXmlRPC::GetEvents(const QList<GetEventsInfo>& infos,
        const QString& journalName, SelectType st, ModelType mt,
        const QString& challenge)
{
    QDomDocument document = GenerateGetEventsRequest(infos, journalName, st,
            challenge);

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    m_CurrentReply = reply;
    connect(reply,
            &QNetworkReply::finished,
            this,
            [this, mt]()
            {
                handleGetEvents(mt);
            });
}

void LJXmlRPC::AddComment(const QString& journalName, quint64 parentTalkId,
        quint64 dItemId, const QString& subject, const QString& body, const QString& avatarId,
        const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document.
            createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.addcomment",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("body",
            "string", body, document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("subject",
            "string", subject, document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("journal",
            "string", journalName, document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("parent",
            "string", QString::number(parentTalkId), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("ditemid",
            "string", QString::number(dItemId), document));
    if (!avatarId.isEmpty())
    {
        element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("picture_keyword",
                "string", avatarId, document));
    }

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleAddComment);
}

void LJXmlRPC::EditComment(const QString& journalName, quint64 dTalkId, const QString& subject,
        const QString& body, const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document.
            createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.editcomment",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("body",
            "string", body, document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("subject",
            "string", subject, document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("journal",
            "string", journalName, document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("dtalkid",
            "string", QString::number(dTalkId), document));

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleEditComment);
}

void LJXmlRPC::DeleteComment(const QString& journalName, quint64 dTalkId, int deleteMask,
        const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document.
            createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.deletecomments",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("journal",
            "string", journalName, document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("dtalkid",
            "string", QString::number(dTalkId), document));

    if (deleteMask & DCTThread)
    {
        element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("thread",
                "boolean", "true", document));
    }

    if (deleteMask & DCTAllComments)
    {
        element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("delauthor",
                "boolean", "true", document));
    }

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleDeleteComment);
}

void LJXmlRPC::GetComments(quint64 dItemId, const QString& journal, int page,
        quint64 dTalkId, const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.getcomments",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("ditemid",
            "int", QString::number(dItemId), document));
    if (!dTalkId)
    {
        element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("page",
                "int", QString::number(page), document));
    }
    else
    {
        element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("thread",
                "int", QString::number(dTalkId), document));
    }
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("journal",
            "string", journal, document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("expand_strategy",
            "string", "mobile_thread", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("page_size",
            "int", QString::number(CommentsPageSize), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("get_users_info",
            "boolean", "true", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("extra",
            "boolean", "true", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("calculate_count",
            "boolean", "true", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("asxml",
            "boolean", "true", document));

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    m_CurrentReply = reply;
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleGetComments);
}

void LJXmlRPC::GetUpdatedComment(quint64 dItemId, const QString& journal, quint64 dTalkId,
        const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.getcomments",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("ditemid",
            "int", QString::number(dItemId), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("dtalkid",
            "int", QString::number(dTalkId), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("journal",
            "string", journal, document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("expand_strategy",
            "string", "detailed", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("get_users_info",
            "boolean", "true", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("extra",
            "boolean", "true", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("calculate_count",
            "boolean", "true", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("asxml",
            "boolean", "true", document));

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleGetUpdatedComment);
}

void LJXmlRPC::UpdateComment(quint64 dItemId, const QString& journalName, quint64 dTalkId,
        LJComment::Privilege action, const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.updatecomments",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("journal",
            "string", journalName, document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("dtalkid",
            "int", QString::number(dTalkId), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("action",
            "string", ToString(action), document));

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            [dItemId, journalName, this]()
            {
                handleUpdateComment(dItemId, journalName);
            });
}

void LJXmlRPC::GetFriendGroups(const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.getfriendgroups",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    m_CurrentReply = reply;
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleGetFriendGroups);
}

void LJXmlRPC::AddFriendGroup(const QString& name, bool isPrivate, int id,
        const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.editfriendgroups",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);
    auto data = RpcUtils::Builder::GetComplexMemberElement("set", "struct",
            document);
    element.appendChild(data.first);
    auto subStruct = RpcUtils::Builder::GetComplexMemberElement(QString::number(id),
            "struct", document);
    data.second.appendChild(subStruct.first);
    subStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement("name",
            "string", name, document));
    subStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement("public",
            "boolean", isPrivate ? "0" : "1", document));

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleAddFriendGroup);
}

void LJXmlRPC::DeleteFriendGroup(quint64 groupId, const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.editfriendgroups",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);

    auto array = RpcUtils::Builder::GetComplexMemberElement("delete", "array",
            document);
    element.appendChild(array.first);
    QDomElement valueField = document.createElement("value");
    array.second.appendChild(valueField);
    QDomElement valueType = document.createElement("int");
    valueField.appendChild(valueType);
    QDomText text = document.createTextNode(QString::number(groupId));
    valueType.appendChild (text);

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleDeleteFriendGroup);
}

void LJXmlRPC::LoadUserJournal(const QString& journalName,
        const QDateTime& before, ModelType mt, const QString& challenge)
{
    QDateTime dt;
    if (!before.isValid())
    {
        dt = QDateTime::currentDateTime();
    }
    else
    {
        dt = before.addSecs(-1);
    }
    QList<GetEventsInfo> infos = {
            { "before", "string", dt.toString("yyyy-MM-dd hh:mm:ss") },
            { "howmany", "int", QString::number(LJXmlRPC::ItemShow) },
            { "sort_order", "string", "desc" },
            { "trim_widgets", "int", QString::number(LJXmlRPC::TrimWidgets) },
            { "widgets_img_length", "int", QString::number(LJXmlRPC::WidgetsImgLength) },
            { "get_video_id", "boolean", "true" },
            { "get_polls", "boolean", "true" },
            { "get_users_info", "boolean", "true"},
            { "view", "boolean", "true" },
            { "asxml", "boolean", "true" } };
    QDomDocument document = GenerateGetEventsRequest(infos, journalName,
            SelectType::Before, challenge);

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    m_CurrentReply = reply;
    connect(reply,
            &QNetworkReply::finished,
            this,
            [this, mt]()
            {
                handleLoadUserJournal(mt);
            });
}

void LJXmlRPC::GetFriends(const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.getfriends",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("includefriendof",
            "boolean", "true", document));

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    m_CurrentReply = reply;
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleGetFriends);
}

void LJXmlRPC::AddFriend(const QString& name, uint groupMask,
        const QString& challenge)
{
    QDomDocument document = GenerateEditFriendsRequest(name, groupMask,
            challenge);

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleAddFriends);
}

void LJXmlRPC::EditFriend(const QString& name, uint groupMask,
        const QString& challenge)
{
    QDomDocument document = GenerateEditFriendsRequest(name, groupMask,
            challenge);

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleEditFriends);
}

void LJXmlRPC::DeleteFriend(const QString& name, const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.editfriends",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);
    auto array = RpcUtils::Builder::GetComplexMemberElement("delete", "array",
            document);
    element.appendChild(array.first);
    QDomElement valueField = document.createElement("value");
    array.second.appendChild(valueField);
    QDomElement valueType = document.createElement("string");
    valueField.appendChild(valueType);
    QDomText text = document.createTextNode(name);
    valueType.appendChild(text);

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleDeleteFriends);
}

void LJXmlRPC::GetMessages(quint64 lastSyncTime, const QString& challenge, const LJMessages_t& msgs)
{
    QDomDocument document = GenerateGetInboxRequest(lastSyncTime,
            {LJMessage::MTUserMessageRecvd, LJMessage::MTUserMessageSent}, challenge);
    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    m_CurrentReply = reply;
    connect(reply,
            &QNetworkReply::finished,
            this,
            [this, msgs]()
            {
                handleGetMessages(msgs);
            });
}

void LJXmlRPC::GetNotifications(quint64 lastSyncTime, const QString& challenge, const LJMessages_t& msgs)
{
    QDomDocument document = GenerateGetInboxRequest(lastSyncTime,
            {  LJMessage::MTJournalNewComment,LJMessage::MTUserNewComment,
                LJMessage::MTUserNewEntry, LJMessage::MTCommentReply }, challenge);
    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    m_CurrentReply = reply;
    connect(reply,
            &QNetworkReply::finished,
            this,
            [this, msgs]()
            {
                handleGetNotifications(msgs);
            });
}

void LJXmlRPC::MarkAsRead(const QList<quint64>& qids, const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.setmessageread",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);
    auto array = RpcUtils::Builder::GetComplexMemberElement("qid", "array",
            document);
    element.appendChild(array.first);
    for (const quint64 qid : qids)
    {
        QDomElement valueField = document.createElement("value");
        array.second.appendChild(valueField);
        QDomElement valueType = document.createElement("int");
        valueField.appendChild(valueType);
        QDomText text = document.createTextNode(QString::number(qid));
        valueType.appendChild(text);
    }

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleMarkAsRead);
}

void LJXmlRPC::SendMessage(const QString& to, const QString& subject, const QString& body,
        const qint64 parentMessageId, const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.sendmessage",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("subject",
            "string", subject, document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("body",
            "string", body, document));
    auto array = RpcUtils::Builder::GetComplexMemberElement("to", "array",
            document);
    element.appendChild(array.first);
    QDomElement valueField = document.createElement("value");
    array.second.appendChild(valueField);
    QDomElement valueType = document.createElement("string");
    valueField.appendChild(valueType);
    QDomText text = document.createTextNode(to);
    valueType.appendChild(text);
    if (parentMessageId != -1)
    {
        element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("parent",
                "int", QString::number(parentMessageId), document));
    }

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleSendMessage);
}

void LJXmlRPC::PostEvent(const LJEvent& event, const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.postevent",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);

    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("event",
            "string", event.GetEvent(), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("subject",
            "string", event.GetSubject(), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("security",
            "string", Utils::AccessToString(event.GetAccess()), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("allowmask",
            "int", QString::number(event.GetAllowMask()), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("usejournal",
            "string", event.GetJournalName(), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("year", "int",
            QString::number(event.GetPostDate().date().year()), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("mon", "int",
            QString::number(event.GetPostDate().date().month()), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("day", "int",
            QString::number(event.GetPostDate().date().day()), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("hour", "int",
            QString::number(event.GetPostDate().time().hour()), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("min", "int",
            QString::number(event.GetPostDate().time().minute()), document));

    auto propsStruct = RpcUtils::Builder::GetComplexMemberElement("props", "struct", document);
    element.appendChild (propsStruct.first);
    propsStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement("opt_nocomments",
            "boolean", QString::number(!event.GetProperties().GetCommentsEnabled()),
            document));
    propsStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement("opt_noemail",
            "boolean", QString::number(!event.GetProperties().GetNotifyByEmail()),
            document));
    propsStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement("adult_content",
            "string",
            Utils::AdultContentToString(event.GetProperties().GetAdultContent()),
            document));
    propsStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement("opt_screening",
            "string",
            Utils::ScreeningToString(event.GetProperties().GetCommentsManagement()),
            document));
    propsStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement("taglist",
            "string", event.GetTags().join(","), document));
    propsStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement("useragent",
            "string", "Harbour Mnemosy", document));
    propsStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement("current_location",
            "string", event.GetProperties().GetCurrentLocation(), document));
    propsStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement ("picture_keyword",
            "string", event.GetProperties().GetPostAvatar(), document));

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handlePostEvent);
}

void LJXmlRPC::EditEvent(const LJEvent& event, const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.editevent",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);

    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("itemid", "int",
            QString::number(event.GetItemID()), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("event",
            "string", event.GetEvent(), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("subject",
            "string", event.GetSubject(), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("security",
            "string", Utils::AccessToString(event.GetAccess()), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("allowmask",
            "int", QString::number(event.GetAllowMask()), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("usejournal",
            "string", event.GetJournalName(), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("year", "int",
            QString::number(event.GetPostDate().date().year()), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("mon", "int",
            QString::number(event.GetPostDate().date().month()), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("day", "int",
            QString::number(event.GetPostDate().date().day()), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("hour", "int",
            QString::number(event.GetPostDate().time().hour()), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("min", "int",
            QString::number(event.GetPostDate().time().minute()), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement ("usejournal", "string",
            event.GetJournalName(), document));

    auto propsStruct = RpcUtils::Builder::GetComplexMemberElement("props", "struct", document);
    element.appendChild (propsStruct.first);
    propsStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement("opt_nocomments",
            "boolean", QString::number(!event.GetProperties().GetCommentsEnabled()),
            document));
    propsStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement("adult_content",
            "string",
            Utils::AdultContentToString(event.GetProperties().GetAdultContent()),
            document));
    propsStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement("opt_screening",
            "string",
            Utils::ScreeningToString(event.GetProperties().GetCommentsManagement()),
            document));
    propsStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement("taglist",
            "string", event.GetTags().join(","), document));
    propsStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement("useragent",
            "string", "Harbour Mnemosy", document));
    propsStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement("current_location",
            "string", event.GetProperties().GetCurrentLocation(), document));
    propsStruct.second.appendChild(RpcUtils::Builder::GetSimpleMemberElement ("picture_keyword",
            "string", event.GetProperties().GetPostAvatar(), document));

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleEditEvent);
}

void LJXmlRPC::DeleteEvent(const quint64 itemId, const QString& journal, const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(xmlHeaderPI);
    auto result = RpcUtils::Builder::GetStartPart("LJ.XMLRPC.editevent",
            document);
    document.appendChild(result.first);

    const auto& login = AccountSettings::Instance(this)->value("login", "")
            .toString();
    const auto& password = AccountSettings::Instance(this)->value("password", "")
            .toString();
    auto element = RpcUtils::Builder::FillServicePart(result.second, login,
            GetPassword(password, challenge), challenge, document);

    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("itemid", "int",
            QString::number(itemId), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("event",
            "string", QString(), document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement ("usejournal", "string",
            journal, document));

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleDeleteEvent);
}

void LJXmlRPC::handleGetChallenge()
{
    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        if (!m_ApiCallQueue.empty())
        {
            m_ApiCallQueue.pop_front();
        }
        emit requestFinished();
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        if (!m_ApiCallQueue.empty())
        {
            m_ApiCallQueue.pop_front();
        }
        emit requestFinished();
        return;
    }

    QXmlQuery query;
    query.setFocus(doc.toString(-1));

    QString challenge;
    query.setQuery("/methodResponse/params/param/value/struct/\
            member[name='challenge']/value/string/text()");
    if (!query.evaluateTo(&challenge))
    {
        qDebug() << Q_FUNC_INFO << "XML data parsing has failed";
        emit error(tr("XML data parsing has failed"));
        m_ApiCallQueue.pop_front();
        emit requestFinished();
        return;
    }

    if (!m_ApiCallQueue.isEmpty())
    {
        m_ApiCallQueue.dequeue()(challenge.simplified());
    }
}

void LJXmlRPC::handleLogin(const QString& login, const QString& password)
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        emit error(tr("Failed to parse reply"));
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    QXmlQuery query;
    query.setFocus(doc.toString(-1));
    QString validated;
    query.setQuery("/methodResponse/params/param/value/struct/\
            member[name='is_validated']/value/int/text()");
    if (query.evaluateTo(&validated))
    {
        const bool isLogged = validated.trimmed() == "1";
        if (isLogged)
        {
            emit gotUserProfile(RpcUtils::Parser::ParseUserProfile(doc));
        }
        else
        {
            emit error(tr("Invalid login or password"), 0, ETLiveJournal);
        }
        emit logged(isLogged, login, password);
    }
    else
    {
       emit error(tr("XML data parsing has failed"));
    }
}

void LJXmlRPC::handleGetFriendsPage()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    emit gotFriendsPage(RpcUtils::Parser::ParseLJEvents(doc));
}

void LJXmlRPC::handleGetEvents(const ModelType mt)
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    emit gotEvent(RpcUtils::Parser::ParseLJEvent(doc), mt);
}

void LJXmlRPC::handleAddComment()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    QXmlQuery query;
    query.setFocus(doc.toString (-1));

    QString status;
    query.setQuery("/methodResponse/params/param/value/struct/"
            "member[name='status']/value/string/text()");
    if (!query.evaluateTo(&status))
    {
        emit error(tr("XML data parsing has failed"));
        return;
    }

    if (status.trimmed().toLower() == "ok")
    {
        emit commentAdded();
    }
    else
    {
        emit error(tr("Comment wasn't added"), 0, ETLiveJournal);
    }
}

void LJXmlRPC::handleEditComment()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    QXmlQuery statusQuery;
    statusQuery.setFocus(doc.toString(-1));

    QString status;
    statusQuery.setQuery("/methodResponse/params/param/value/struct/"
            "member[name='status']/value/string/text()");
    if (!statusQuery.evaluateTo(&status))
    {
        emit error(tr("XML data parsing has failed"));
        return;
    }

    if (status.trimmed().toLower() == "ok")
    {
        QXmlQuery dtalkidQuery;
        dtalkidQuery.setFocus(doc.toString(-1));
        QString dTalkId;
        dtalkidQuery.setQuery("/methodResponse/params/param/value/struct/"
                "member[name='dtalkid']/value/int/text()");
        if (!dtalkidQuery.evaluateTo(&dTalkId))
        {
            emit error(tr("XML data parsing has failed"));
            return;
        }

        emit commentEdited(dTalkId.toInt());
    }
    else
    {
        emit error(tr("Comment wasn't edited"), 0, ETLiveJournal);
    }
}

void LJXmlRPC::handleDeleteComment()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    QXmlQuery query;
    query.setFocus(doc.toString (-1));

    QString status;
    query.setQuery("/methodResponse/params/param/value/struct/"
            "member[name='status']/value/string/text()");
    if (!query.evaluateTo(&status))
    {
        emit error(tr("XML data parsing has failed"));
        return;
    }

    if (status.trimmed().toLower() == "ok")
    {
        emit commentsDeleted(RpcUtils::Parser::ParseLJDeletedComments(doc));
    }
    else
    {
        emit error(tr("Comment wasn't deleted"), 0, ETLiveJournal);
    }
}

void LJXmlRPC::handleGetComments()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    const auto& postComments = RpcUtils::Parser::ParseLJPostComments(doc);
    emit gotComments(postComments);
}

void LJXmlRPC::handleGetUpdatedComment()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    const auto& comments = RpcUtils::Parser::ParseLJPostComments(doc);
    emit commentsUpdated(comments);
}

void LJXmlRPC::handleUpdateComment(quint64 dItemId, const QString& journalName)
{
    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    QXmlQuery query;
    query.setFocus(doc.toString (-1));

    QString status;
    query.setQuery("/methodResponse/params/param/value/struct/"
            "member[name='status']/value/string/text()");
    if (!query.evaluateTo(&status))
    {
        emit error(tr("XML data parsing has failed"));
        return;
    }

    if (status.trimmed().toLower() == "ok")
    {
        auto list = RpcUtils::Parser::ParseLJUpdatedComments(doc);
        if (list.isEmpty())
        {
            emit requestFinished();
            emit commentsUpdated();
        }
        else
        {
            GetUpdatedComment(dItemId, journalName, list.at(0));
        }
    }
    else
    {
        emit requestFinished();
        emit error(tr("Comment wasn't updated"), 0, ETLiveJournal);
    }
}

void LJXmlRPC::handleGetFriendGroups()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    emit gotFriendGroups(RpcUtils::Parser::ParseFriendGroups(doc));
}

void LJXmlRPC::handleAddFriendGroup()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }
    emit groupAdded();
}

void LJXmlRPC::handleDeleteFriendGroup()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    emit groupDeleted();
}

void LJXmlRPC::handleLoadUserJournal(const ModelType mt)
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    emit gotEvents(RpcUtils::Parser::ParseLJEvents(doc), mt);
}

void LJXmlRPC::handleGetFriends()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    emit gotFriends(RpcUtils::Parser::ParseLJFriends(doc));
}

void LJXmlRPC::handleAddFriends()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    emit friendAdded(RpcUtils::Parser::ParseLJFriends(doc).value(0));
}

void LJXmlRPC::handleEditFriends()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    emit friendEdited();
}

void LJXmlRPC::handleDeleteFriends()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    emit friendDeleted();
}

void LJXmlRPC::handleGetMessages(const LJMessages_t& prevMsgs)
{
    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    LJMessages_t msgs = RpcUtils::Parser::ParseLJMessages(doc);
    std::reverse(std::begin(msgs), std::end(msgs));
    if (!msgs.count())
    {
        emit requestFinished();
        emit gotMessages(prevMsgs);
    }
    else
    {
        msgs = msgs + prevMsgs;
        GetMessages(msgs.first().GetDate().toTime_t() + 1, msgs);
    }
}

void LJXmlRPC::handleGetNotifications(const LJMessages_t& prevMsgs)
{
    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    LJMessages_t msgs = RpcUtils::Parser::ParseLJMessages(doc);
    std::reverse(std::begin(msgs), std::end(msgs));
    if (!msgs.count())
    {
        emit requestFinished();
        emit gotNotifications(prevMsgs);
    }
    else
    {
        msgs = msgs + prevMsgs;
        GetNotifications(msgs.first().GetDate().toTime_t() + 1, msgs);
    }
}

void LJXmlRPC::handleMarkAsRead()
{
    emit requestFinished();
    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    emit gotReadMessages(RpcUtils::Parser::ParseReadMessages(doc));
}

void LJXmlRPC::handleSendMessage()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    emit messageSent();
}

void LJXmlRPC::handlePostEvent()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    emit eventPosted();
}

void LJXmlRPC::handleEditEvent()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    const QPair<quint64, quint64> itemIds = RpcUtils::Parser::ParseEditedEvent(doc);
    if (itemIds.first <= 0 || itemIds.second <= 0)
    {
        emit error(tr("Unabele to get item from server answer"), 0, ETLiveJournal);
        return;
    }

    emit eventEdited(itemIds.first, itemIds.second);
}

void LJXmlRPC::handleDeleteEvent()
{
    emit requestFinished();

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), ok);
    if (!ok)
    {
        qDebug() << Q_FUNC_INFO << "Failed preparsing reply phase";
        return;
    }

    const auto& result = CheckOnLJErrors(doc);
    if (result.first)
    {
        qDebug() << Q_FUNC_INFO << "There is error from LJ: code ="
                << result.first << "description =" << result.second;
        emit error(result.second, result.first, ETLiveJournal);
        return;
    }

    eventDeleted(RpcUtils::Parser::ParseDeletedEvent(doc));
}
} // namespace Mnemosy
