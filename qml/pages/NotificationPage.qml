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

import "../components"
import "../utils/Utils.js" as Utils

Page {
    id: messagePage

    property variant notification

    function attachPage() {
        if (pageStack._currentContainer.attachedContainer === null &&
                mnemosyManager.logged) {
            pageStack.pushAttached(Qt.resolvedUrl("ProfilePage.qml"))
        }
    }

    onStatusChanged: {
        if (status == PageStatus.Active) {
            attachPage()
            mnemosyManager.markMessageAsRead(notification.messageQId)
        }
    }

    SilicaFlickable {
        id: messageView

        anchors.fill: parent

        contentWidth: width
        contentHeight: column.height + Theme.paddingSmall

        clip: true

        PullDownMenu {
            MenuItem {
                text: qsTr("Open entry")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("EventPage.qml"),
                            { modelType: Mnemosy.UserModel,
                                dItemId: notification.messageEntryDItemId,
                                journalName: notification.messageJournalName,
                                userPicUrl: notification.messagePosterAvatar })
                }
            }

            MenuItem {
                text: qsTr("Reply")
                onClicked: {
                    var dialog = pageStack.push("../dialogs/AddEditCommentDialog.qml")
                    dialog.accepted.connect(function () {
                        mnemosyManager.addComment(notification.messageJournalName,
                                notification.messageDTalkId, notification.messageEntryDItemId,
                                dialog.subject, dialog.body, dialog.avatarId)
                    })
                }
            }
        }

        Column {
            id: column

            spacing: Theme.paddingSmall

            width: parent.width

            anchors.top: parent.top
            anchors.topMargin: Theme.paddingSmall
            anchors.left: parent.left
            anchors.leftMargin: Theme.horizontalPageMargin
            anchors.right: parent.right
            anchors.rightMargin: Theme.horizontalPageMargin

            EntryHeaderItem {
                width: parent.width

                posterAvatar: notification.messagePosterAvatar
                posterName: notification.messagePosterName.toUpperCase()
                postDate: Utils.generateDateString(notification.messagePostDate, "dd MMM yyyy hh:mm")
            }

            Label {
                id: subjectLabel

                width: parent.width

                wrapMode: Text.WordWrap

                font.pixelSize: Theme.fontSizeMedium
                font.family: Theme.fontFamilyHeading
                font.bold: true

                text: notification.messageSubject.length > 0 ?
                        notification.messageSubject :
                        qsTr("(Without subject)")
            }

            Label {
                id: messageLabel

                width: parent.width

                wrapMode: Text.WordWrap
                horizontalAlignment: Qt.AlignJustify

                font.pixelSize: Theme.fontSizeSmall
                text: notification.messageBody

                onLinkActivated: {
                    Qt.openUrlExternally(link)
                }
            }
        }

        VerticalScrollDecorator {}
    }
}
