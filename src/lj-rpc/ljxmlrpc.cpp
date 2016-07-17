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
#include <QCoreApplication>
#include <QDomElement>
#include <QDomNode>
#include <QDomProcessingInstruction>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QXmlQuery>

#include "src/lj-rpc/rpcutils.h"
#include "src/settings/applicationsettings.h"

namespace Mnemosy
{
LJXmlRPC::LJXmlRPC(QObject *parent)
: QObject(parent)
, m_NAM(new QNetworkAccessManager(this))
{
}

void LJXmlRPC::Login(const QString& login, const QString& password)
{
    auto guard = MakeRunnerGuard();
    m_ApiCallQueue << [this](const QString&){ GetChallenge(); };
    m_ApiCallQueue << [login, password, this](const QString& challenge)
        { Login(login, password, challenge); };
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

QDomDocument LJXmlRPC::ParseDocument(const QByteArray& data, bool& ok)
{
    QDomDocument document;
    QString errorMsg;
    int errorLine = -1;
    int errorColumn = -1;
    if (!document.setContent(data, &errorMsg, &errorLine, &errorColumn))
    {
        qWarning() << Q_FUNC_INFO
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
    m_Reply2LoginPassword.insert(reply, { login, password });
    connect(reply,
            &QNetworkReply::finished,
            this,
            &LJXmlRPC::handleLogin);
}

namespace
{
QPair<int, QString> CheckOnError(const QDomDocument& doc)
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

    return qMakePair(errorCode.toInt(), errorString);
}
}

void LJXmlRPC::handleGetChallenge()
{
    qDebug() << Q_FUNC_INFO;
    auto reply = qobject_cast<QNetworkReply*> (sender());
    if (!reply)
    {
        qDebug() << "Invalid reply";
        return;
    }
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << Q_FUNC_INFO << "There is network error: "
                << reply->error() << reply->errorString();
        emit requestFinished(false, tr("Network error"));
        m_ApiCallQueue.pop_front();
        return;
    }

    const QByteArray& data = reply->readAll();
    bool ok = false;
    QDomDocument doc = ParseDocument(data, ok);
    if (!ok)
    {
        qWarning() << "Unable to generate xml from reply";
        emit requestFinished(false, tr("Reply data is corrupted"));
        m_ApiCallQueue.pop_front();
        return;
    }

    const auto& result = CheckOnError(doc);
    if (result.first)
    {
        emit requestFinished(true);
        //TODO emit error with code and message
        return;
    }
    QXmlQuery query;
    query.setFocus(doc.toString(-1));

    QString challenge;
    query.setQuery("/methodResponse/params/param/value/struct/\
            member[name='challenge']/value/string/text()");
    if (!query.evaluateTo(&challenge))
    {
        //TODO error with popup and set busy to false
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

void LJXmlRPC::handleLogin()
{
    qDebug() << Q_FUNC_INFO;
    auto reply = qobject_cast<QNetworkReply*> (sender());
    if (!reply)
    {
        qDebug() << "Invalid reply";
        return;
    }
    reply->deleteLater();
    const auto authData = m_Reply2LoginPassword.take(reply);

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << Q_FUNC_INFO << "There is network error: "
                << reply->error() << reply->errorString();
        emit requestFinished(false, tr("Network error"));
        return;
    }

    const QByteArray& data = reply->readAll();
    bool ok = false;
    QDomDocument doc = ParseDocument(data, ok);
    if (!ok)
    {
        qWarning() << "Unable to generate xml from reply";
        emit requestFinished(false, tr("Reply data is corrupted"));
        return;
    }

    const auto& result = CheckOnError(doc);
    if (result.first)
    {
        emit requestFinished(true);
        if (result.first == 100 || result.first == 101)
        {
            emit logged(false, authData.first, authData.second);
        }
        else
        {
            //TODO emit error with code and message
        }
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
            qDebug () << doc.toByteArray();
            emit gotUserProfile(RpcUtils::Parser::ParseUserProfile(doc));
            emit requestFinished(true);
        }
        else
        {
            emit requestFinished(false, tr("Invalid login or password"));
        }
        emit logged(isLogged, authData.first, authData.second);
    }
    else
    {
        emit requestFinished(false, tr("XML data parsing has failed"));
    }
}
} // namespace Mnemosy
