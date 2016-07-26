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

#include "application.h"

#include "sailfishapp.h"

#include <QQuickView>
#include <QQmlContext>
#include <QtDebug>
#include <QtQml>

#include "enumsproxy.h"
#include "mnemosymanager.h"
#include "models/ljeventsmodel.h"
#include "settings/accountsettings.h"
#include "userprofile.h"

namespace Mnemosy
{
    Application::Application(QObject *parent)
    : QObject(parent)
    , m_View(nullptr)
    {
    }

    void Application::ShowUI()
    {
        if (!m_View)
        {
            qDebug() << "Construct view";
            m_View = SailfishApp::createView();
            m_View->setTitle("Mnemosy");
            m_View->rootContext()->setContextProperty("accountSettings",
                    AccountSettings::Instance(this));
            m_View->rootContext()->setContextProperty("mnemosyManager",
                    MnemosyManager::Instance(this));
            m_View->setSource(SailfishApp::pathTo("qml/harbour-mnemosy.qml"));
            m_View->showFullScreen();
        }
        else
        {
            qDebug() << "Activating view";
            m_View->raise();
            m_View->requestActivate();
        }
    }

    void Application::start()
    {
        qRegisterMetaType<UserProfile*>("UserProfile*");
        qRegisterMetaType<LJEventsModel*>("LJEventsModel*");

        qmlRegisterUncreatableType<MnemosyManager>("org.mnemosy", 1, 0,
                "MnemosyManager", "MnemosyManager static uncreatable type");
        qmlRegisterUncreatableType<Mnemosy::EnumsProxy>("org.mnemosy", 1, 0,
                "Mnemosy", "This exports otherwise unavailable \
                        Mnemosy datatypes to QML");
        qmlRegisterUncreatableType<UserProfile>("org.mnemosy", 1, 0,
                "UserProfile", "UserProfile uncreatable type");

        ShowUI();
    }
} // namespace Mnemosy
