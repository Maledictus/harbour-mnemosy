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

#include "ljxmlrpc.h"
#include <QDomElement>
#include <QDomNode>
#include <QDomProcessingInstruction>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QXmlQuery>

#include "src/lj-rpc/rpcutils.h"
#include "src/settings/accountsettings.h"

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
        quint64 dItemId, const QString& subject, const QString& body)
{
    auto guard = MakeRunnerGuard();
    m_ApiCallQueue << [this](const QString&){ GetChallenge(); };
    m_ApiCallQueue << [journalName, parentTalkId, dItemId, subject, body, this]
            (const QString& challenge)
        {
            AddComment(journalName, parentTalkId, dItemId, subject, body,
                    challenge);
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

void LJXmlRPC::DeleteComment(const QString& journalName, quint64 dTalkId)
{
    auto guard = MakeRunnerGuard();
    m_ApiCallQueue << [this](const QString&){ GetChallenge(); };
    m_ApiCallQueue << [journalName, dTalkId, this]
            (const QString& challenge)
        {
            DeleteComment(journalName, dTalkId, challenge);
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
    m_ApiCallQueue << [this] (const QString&) { GetChallenge (); };
    m_ApiCallQueue << [journalName, before, mt, this] (const QString& challenge)
        { LoadUserJournal(journalName, before, mt, challenge); };
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

QDomDocument LJXmlRPC::PreparsingReply(QObject* sender, bool popFromQueue, bool& ok)
{
    QDomDocument doc;
    auto reply = qobject_cast<QNetworkReply*> (sender);
    if (!reply)
    {
        qDebug() << "Invalid reply";
        emit requestFinished(false, tr("General error"));
        ok = false;
        return doc;
    }
    reply->deleteLater();
    m_CurrentReply.clear();

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << Q_FUNC_INFO << "There is network error: "
                << reply->error() << reply->errorString();
        emit requestFinished(false, tr("Network error %1: %2")
                .arg(reply->error())
                .arg(reply->errorString()));
        if (popFromQueue)
        {
            m_ApiCallQueue.pop_front();
        }
        ok = false;
        return doc;
    }
    QByteArray data = reply->readAll();
    ok = false;
    doc = ParseDocument(data, ok);
    if (!ok)
    {
        qDebug() << "Unable to generate xml from reply";
        emit requestFinished(false, tr("Reply data is corrupted"));
        if (popFromQueue)
        {
            m_ApiCallQueue.pop_front();
        }
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
        emit requestFinished(true);
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
    request.setUrl(QUrl("http://www.livejournal.com/interface/xmlrpc"));
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
            "string", QString::number(before.toTime_t() - 1), document));
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
        quint64 dItemId, const QString& subject, const QString& body,
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

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    m_CurrentReply = reply;
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleAddComment);
}

void LJXmlRPC::EditComment(const QString& journalName, quint64 dTalkId, const QString& subject, const QString& body, const QString& challenge)
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
    m_CurrentReply = reply;
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleEditComment);
}

void LJXmlRPC::DeleteComment(const QString& journalName, quint64 dTalkId, const QString& challenge)
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

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    m_CurrentReply = reply;
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
            .createProcessingInstruction ("xml", "version=\"1.0\" encoding=\"UTF-8\"");
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
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("expang_strategy",
            "string", "mobile_thread", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("get_users_info",
            "boolean", "true", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("extra",
            "boolean", "true", document));
    element.appendChild(RpcUtils::Builder::GetSimpleMemberElement("calculate_count",
            "boolean", "true", document));

    auto reply = m_NAM->post(CreateNetworkRequest(), document.toByteArray());
    m_CurrentReply = reply;
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleGetComments);
}

void LJXmlRPC::GetFriendGroups(const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction ("xml", "version=\"1.0\" encoding=\"UTF-8\"");
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
            .createProcessingInstruction ("xml", "version=\"1.0\" encoding=\"UTF-8\"");
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
    m_CurrentReply = reply;
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleAddFriendGroup);
}

void LJXmlRPC::DeleteFriendGroup(quint64 groupId, const QString& challenge)
{
    QDomDocument document;
    QDomProcessingInstruction xmlHeaderPI = document
            .createProcessingInstruction ("xml", "version=\"1.0\" encoding=\"UTF-8\"");
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
    m_CurrentReply = reply;
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleDeleteFriendGroup);
}

void LJXmlRPC::LoadUserJournal(const QString& journalName,
        const QDateTime& before, ModelType mt, const QString& challenge)
{
    QDateTime dt = before.addSecs(-1);
    QList<GetEventsInfo> infos = {
            { "before", "string", dt.toString("yyyy-MM-dd hh:mm:ss") },
            { "itemshow", "int", QString::number(LJXmlRPC::ItemShow) },
            { "sort_order", "string", "desc" },
            { "trim_widgets", "int", QString::number(LJXmlRPC::TrimWidgets) },
            { "widgets_o,g_length", "int", QString::number(LJXmlRPC::WidgetsImgLength) } };
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

void LJXmlRPC::handleGetChallenge()
{
    qDebug() << Q_FUNC_INFO;

    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), true, ok);
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
    query.setFocus(doc.toString(-1));

    QString challenge;
    query.setQuery("/methodResponse/params/param/value/struct/\
            member[name='challenge']/value/string/text()");
    if (!query.evaluateTo(&challenge))
    {
        emit requestFinished(false, tr("XML data parsing has failed"));
        qDebug() << Q_FUNC_INFO << "XML data parsing has failed";
        m_ApiCallQueue.pop_front();
        return;
    }

    if (!m_ApiCallQueue.isEmpty())
    {
        m_ApiCallQueue.dequeue()(challenge.simplified());
    }
}

void LJXmlRPC::handleLogin(const QString& login, const QString& password)
{
    qDebug() << Q_FUNC_INFO;
    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), false, ok);
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
            emit requestFinished(true);
        }
        else
        {
            emit requestFinished(false, tr("Invalid login or password"));
        }
        emit logged(isLogged, login, password);
    }
    else
    {
        emit requestFinished(false, tr("XML data parsing has failed"));
    }
}

void LJXmlRPC::handleGetFriendsPage()
{
    qDebug() << Q_FUNC_INFO;
    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), false, ok);
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
    emit requestFinished(true);
}

void LJXmlRPC::handleGetEvents(const ModelType mt)
{
    qDebug() << Q_FUNC_INFO;
    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), false, ok);
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
    emit requestFinished(true);
}

void LJXmlRPC::handleAddComment()
{
    qDebug() << Q_FUNC_INFO;
    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), false, ok);
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
        emit requestFinished(false, tr("XML data parsing has failed"));
        return;
    }

    if (status.trimmed().toLower() == "ok")
    {
        emit requestFinished(true);
        emit commentAdded();
    }
    else
    {
        emit requestFinished(false, tr("Comment wasn't added"));
    }
}

void LJXmlRPC::handleEditComment()
{
    qDebug() << Q_FUNC_INFO;
    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), false, ok);
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
        emit requestFinished(false, tr("XML data parsing has failed"));
        return;
    }

    if (status.trimmed().toLower() == "ok")
    {
        emit requestFinished(true);
        emit commentEdited();
    }
    else
    {
        emit requestFinished(false, tr("Comment wasn't edited"));
    }
}

void LJXmlRPC::handleDeleteComment()
{
    qDebug() << Q_FUNC_INFO;
    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), false, ok);
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
        emit requestFinished(false, tr("XML data parsing has failed"));
        return;
    }

    if (status.trimmed().toLower() == "ok")
    {
        emit requestFinished(true);
        emit commentDeleted();
    }
    else
    {
        emit requestFinished(false, tr("Comment wasn't deleted"));
    }
}

void LJXmlRPC::handleGetComments()
{
    qDebug() << Q_FUNC_INFO;
    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), false, ok);
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

    emit requestFinished(true);
    const auto& postComments = RpcUtils::Parser::ParseLJPostComments(doc);
    emit commentsCountChanged(postComments.m_DItemId, postComments.m_CommentsCount);
    emit gotComments(postComments);
}

void LJXmlRPC::handleGetFriendGroups()
{
    qDebug() << Q_FUNC_INFO;
    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), false, ok);
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

    emit requestFinished(true);
    emit gotFriendGroups(RpcUtils::Parser::ParseFriendGroups(doc));
}

void LJXmlRPC::handleAddFriendGroup()
{
    qDebug() << Q_FUNC_INFO;
    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), false, ok);
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
    emit requestFinished(true);
    emit groupAdded();
}

void LJXmlRPC::handleDeleteFriendGroup()
{
    qDebug() << Q_FUNC_INFO;
    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), false, ok);
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

    emit requestFinished(true);
    emit groupDeleted();
}

void LJXmlRPC::handleLoadUserJournal(const ModelType mt)
{
    qDebug() << Q_FUNC_INFO;
    bool ok = false;
    QDomDocument doc = PreparsingReply(sender(), false, ok);
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
    emit requestFinished(true);
}
} // namespace Mnemosy
