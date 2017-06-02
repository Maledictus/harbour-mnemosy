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
import QtQuick.Layouts 1.1
import Sailfish.Silica 1.0
import harbour.mnemosy 1.0

import "../utils"
import "../utils/Utils.js" as Utils

Page {
    id: messagesPage

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

    property MessageDirectionFilter messageDirectionFilter: getDirectionByKey(applicationSettings
            .value("messageDirection", "in"))

    function getDirectionByKey(key) {
        for (var i = 0; i < messageDirectionFilters.length; ++i) {
            if (messageDirectionFilters[i].key === key) {
                return messageDirectionFilters[i]
            }
        }

        return inboxFilter;
    }


    property MessageDirectionFilter inboxFilter: MessageDirectionFilter {
        name: qsTr("Inbox")
        key: "in"
    }

    property MessageDirectionFilter outboxFilter: MessageDirectionFilter {
        name: qsTr("Outbox")
        key: "out"
    }

    property variant messageDirectionFilters: [
        inboxFilter,
        outboxFilter
    ]

    onMessageDirectionFilterChanged: {
        if (messageDirectionFilter.key === "in")
        {
            mnemosyManager.messagesModel.filterMessages(Mnemosy.Inbox)
        }
        else if (messageDirectionFilter.key === "out")
        {
            mnemosyManager.messagesModel.filterMessages(Mnemosy.Outbox)
        }

        applicationSettings.setValue("messageDirection", messageDirectionFilter.key)
    }

    SilicaListView {
        id: messagesView

        anchors.fill: parent
        header: PageHeader {
            title: qsTr("Messages")
        }

        ViewPlaceholder {
            enabled: !mnemosyManager.busy && !messagesView.count
            text: qsTr("There are no messages. Pull down to refresh")
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Mark all as read")
                visible: messagesView.count > 0
                onClicked: {
                    mnemosyManager.markAllMessagesAsRead()
                }
            }

            MenuItem {
                text: qsTr("Show: %1").arg(messageDirectionFilter.name)
                onClicked: {
                    pageStack.push (Qt.resolvedUrl("MessagesFilterSelectorPage.qml"),
                            { parentPage: messagesPage });
                }
            }

            MenuItem {
                text: qsTr("Refresh")
                onClicked: {
                    mnemosyManager.getMessages()
                }
            }
        }

        PushUpMenu {
            MenuItem {
                text: qsTr("Go to top")
                onClicked: {
                    messagesView.scrollToTop()
                }
            }

            visible: !mnemosyManager.busy && messagesView.count > 0
        }

        spacing: Theme.paddingSmall

        model: mnemosyManager.messagesModel

        delegate: ListItem {
            id: rootDelegateItem

            width: messagesView.width

            contentHeight: contentItem.childrenRect.height + Theme.paddingSmall

            menu: ContextMenu {
                MenuItem {
                    visible: messageState === Mnemosy.Unread
                    text: qsTr("Mark as read")
                    onClicked: {
                        mnemosyManager.markMessageAsRead(messageQId)
                    }
                }

                MenuItem {
                    visible: messageDirection === Mnemosy.Inbox
                    text: qsTr("Reply")
                    onClicked: {
                        var dialog = pageStack.push("../dialogs/NewMessageDialog.qml")
                        dialog.accepted.connect (function () {
                            mnemosyManager.sendMessage(messageFrom,
                                    dialog.subject, dialog.body, messageId) })
                    }
                }
            }

            Image {
                id: posterPicUrl

                sourceSize.height: 96
                sourceSize.width: 96

                anchors.left: parent.left
                anchors.leftMargin: Theme.horizontalPageMargin
                anchors.top: parent.top
                anchors.topMargin: Theme.paddingSmall

                source: {
                    return messagePosterAvatar.toString() === "" ?
                        "qrc:/images/blank_boy.png" :
                        messagePosterAvatar
                }

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

                RowLayout {
                    width: parent.width
                    Label {
                        id: from

                        anchors.left: parent.left
                        anchors.right: date.left
                        anchors.rightMargin: Theme.paddingSmall

                        text: messageFrom === "" ? mnemosyManager.profile.userName : messageFrom
                        font.bold: true
                        Layout.alignment: Qt.AlignLeft
                    }

                    Label {
                        id: date

                        Layout.alignment: Qt.AlignRight

                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: Theme.primaryColor

                        text: Utils.generateDateString(messagePostDate, "dd MMM hh:mm")
                    }
                }

                Label {
                    id: subject

                    width: parent.width

                    elide: Text.ElideRight
                    wrapMode: Text.WordWrap
                    maximumLineCount: 1
                    text: messageSubject
                    font.bold: true
                    font.pixelSize: Theme.fontSizeSmall
                    font.family: Theme.fontFamilyHeading
                }

                Label {
                    id: body

                    width: parent.width

                    elide: Text.ElideRight
                    wrapMode: Text.WordWrap
                    maximumLineCount: 3
                    text: messageBody
                    font.pixelSize: Theme.fontSizeSmall
                }
            }

            onClicked: {
                var poster = messageFrom === "" ?
                    mnemosyManager.profile.userName : messageFrom
                pageStack.push(Qt.resolvedUrl("MessagePage.qml"),
                        { messageQId: messageQId, messageId: messageId, posterName: poster,
                            avatarUrl: messagePosterAvatar,
                            subject: messageSubject, body: messageBody,
                            postDate: Utils.generateDateString(messagePostDate, "dd MMM yyyy hh:mm") })
            }
        }

        VerticalScrollDecorator {}
    }
}
