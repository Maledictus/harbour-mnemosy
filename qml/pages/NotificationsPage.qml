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

import "../utils/Utils.js" as Utils

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
            text: qsTr("There are no notifications. Pull down to refresh")
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Mark all as read")
                visible: notificationsView.count > 0
                onClicked: {
                    mnemosyManager.markAllNotificationsAsRead()
                }
            }
            MenuItem {
                text: qsTr("Refresh")
                onClicked: {
                    mnemosyManager.getNotifications()
                }
            }
        }

        PushUpMenu {
            MenuItem {
                text: qsTr("Go to top")
                onClicked: {
                    notificationsView.scrollToTop()
                }
            }

            visible: !mnemosyManager.busy && notificationsView.count > 0
        }

        spacing: Theme.paddingSmall

        model: mnemosyManager.notificationsModel

        delegate: ListItem {
            id: rootDelegateItem

            width: notificationsView.width

            contentHeight: contentItem.childrenRect.height + Theme.paddingSmall

            menu: ContextMenu {
                MenuItem {
                    visible: messageState === Mnemosy.Unread
                    text: qsTr("Mark as read")
                    onClicked: {
                        mnemosyManager.markNotificationAsRead(messageQId)
                    }
                }
            }

            Image {
                id: posterPicUrl

                width: 96
                height: 96

                anchors.left: parent.left
                anchors.leftMargin: Theme.horizontalPageMargin
                anchors.top: parent.top
                anchors.topMargin: Theme.paddingSmall

                sourceSize.height: height
                sourceSize.width: width
                source: messagePosterAvatar

                onStatusChanged: {
                    if (status === Image.Error) {
                        source = "qrc:/images/blank_boy.png"
                    }
                }
            }

            Image {
                id: unreadLogo
                visible: messageState === Mnemosy.Unread
                source: "image://theme/icon-s-high-importance"

                x: posterPicUrl.x - sourceSize.width / 2
                y: posterPicUrl.y - sourceSize.height / 4
                z: posterPicUrl.z + 1
            }

            Column {
                anchors.left: posterPicUrl.right
                anchors.leftMargin: Theme.paddingSmall
                anchors.right: parent.right
                anchors.rightMargin: Theme.horizontalPageMargin

                spacing: Theme.paddingSmall

                Item {
                    width: parent.width
                    height: from.contentHeight
                    Label {
                        id: from

                        anchors.left: parent.left
                        anchors.right: date.left
                        anchors.rightMargin: Theme.paddingSmall

                        text: messagePosterName
                        font.bold: true
                    }

                    Label {
                        id: date

                        anchors.right: parent.right
                        anchors.rightMargin: Theme.paddingSmall

                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: Theme.primaryColor

                        text: Utils.generateDateString(messagePostDate, "dd MMM hh:mm")
                    }
                }

                Label {
                    id: body

                    width: parent.width

                    text: {
                        if (messageType === Mnemosy.JournalNewComment ||
                                messageType === Mnemosy.CommentReply) {
                            if (messageAction === Mnemosy.Deleted ||
                                    messageAction === Mnemosy.CommentDeleted) {
                                return qsTr("Comment deleted")
                            }
                            else if (messageAction === Mnemosy.Edited) {
                                return qsTr("Comment edited")
                            }
                            else if (messageAction === Mnemosy.New) {
                                return qsTr("New comment")
                            }
                        }
                        else if (messageType === Mnemosy.UserNewComment ||
                                messageType === Mnemosy.CommentReply) {
                            return qsTr("New comment")
                        }
                        else if (messageType === Mnemosy.UserNewEntry) {
                            return qsTr("New entry")
                        }

                        return ""
                    }
                }
            }

            onClicked: {
                if (messageAction === Mnemosy.Deleted ||
                        messageAction === Mnemosy.CommentDeleted) {
                    return
                }
                pageStack.push(Qt.resolvedUrl("NotificationPage.qml"),
                        { notification: mnemosyManager.notificationsModel.get(index) })
            }
        }

        VerticalScrollDecorator {}
    }
}
