/*
The MIT License (MIT)

Copyright (c) 2016-2017 Oleg Linkin <maledictusdemagog@gmail.com>

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

Page {
    id: notificationsPage

    BusyIndicator {
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        running: mnemosyManager.busy
        visible: running
    }

    function attachPage() {
        if (pageStack._currentContainer.attachedContainer === null &&
                mnemosyManager.logged) {
            pageStack.pushAttached(Qt.resolvedUrl("ProfilePage.qml"))
        }
    }

    onStatusChanged: {
        if (status == PageStatus.Active && mnemosyManager.logged) {
            attachPage()
        }
    }

    SilicaListView {
        id: notificationsView

        anchors.fill: parent
        header: PageHeader {
            title: qsTr("Notifications")
        }

        ViewPlaceholder {
            enabled: !mnemosyManager.busy && !notificationsView.count
            text: qsTr("There are no messages. Pull down to refresh")
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Refresh")
                onClicked: {
                    mnemosyManager.getNotifications()
                }
            }
            MenuItem {
                text: qsTr("Mark all as read")
                onClicked: {
                    mnemosyManager.markAllNotificationsAsRead()
                }
            }
        }

        spacing: Theme.paddingMedium

        model: mnemosyManager.notificationsModel

        delegate: ListItem {
            id: rootDelegateItem

            width: notificationsView.width

            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Mark as read")
                    onClicked: {
                        //TODO
                    }
                }
            }


        }

        VerticalScrollDecorator {}
    }
}