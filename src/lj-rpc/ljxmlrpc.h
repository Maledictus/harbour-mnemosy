/*
The MIT License (MIT)

Copyright (c) 2016 Oleg Linkin <maledictusdemagog@gmail.com>

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

#include <functional>
#include <memory>
#include <QDomDocument>
#include <QObject>
#include <QMap>
#include <QPair>
#include <QQueue>
#include "src/userprofile.h"

class QNetworkAccessManager;
class QNetworkReply;

namespace Mnemosy
{
class LJXmlRPC : public QObject
{
    Q_OBJECT

    QNetworkAccessManager *m_NAM;
    QQueue<std::function<void (const QString&)>> m_ApiCallQueue;
    QMap<QNetworkReply*, QPair<QString, QString>> m_Reply2LoginPassword;
public:
    explicit LJXmlRPC(QObject *parent = 0);

    void Login(const QString& login, const QString& password);
private:
    std::shared_ptr<void> MakeRunnerGuard();

    QDomDocument ParseDocument(const QByteArray& data, bool& ok);

    void GetChallenge();
    void Login(const QString& login, const QString& password,
            const QString& challenge);

private slots:
    void handleGetChallenge();
    void handleLogin();

signals:
    void requestFinished(bool success = true, const QString& errorMsg = QString());
    void logged(bool result, const QString& login, const QString& password);
    void gotUserProfile(UserProfile *profile);
};
} // namespace Mnemosy
