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

import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.mnemosy 1.0
import Nemo.Notifications 1.0

import "components"
import "cover"
import "dialogs"
import "pages"

ApplicationWindow {
    id: mainWindow

    cover: CoverPage { }

    _defaultPageOrientations: Orientation.Landscape | Orientation.Portrait
    initialPage: accountSettings.value("login", "").length > 0 &&
                 accountSettings.value("password", "").length > 0 ?
            splashScreenComponent :
            authorizationDialogComponent

    Notification {
        id: notification
    }

    Connections {
        target: mnemosyManager
        onLoggedChanged: {
            if (!mnemosyManager.logged)
            {
                pageStack.clear()
                pageStack.push(Qt.resolvedUrl("dialogs/AuthorizationDialog.qml"))
            }
            else {
                pageStack.currentPage.onSuccessLogin()
            }
        }

        onError: {
            notification.previewSummary = msg
            notification.icon = "image://Theme/icon-system-warning"
            notification.publish()
        }

        onNotify: {
            notification.previewSummary = msg
            notification.icon = "image://Theme/icon-system-resources"
            notification.publish()
        }
    }

    Component {
        id: splashScreenComponent

        SplashScreenPage {}
    }

    Component {
        id: authorizationDialogComponent

        AuthorizationDialog {}
    }
}

