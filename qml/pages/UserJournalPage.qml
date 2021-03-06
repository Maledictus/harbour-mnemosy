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

import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.mnemosy 1.0

import "../components"
import "../utils/Utils.js" as Utils

Page {
    id: userJournalPage
    property bool busy: mnemosyManager.busy && userJournalPage.status === PageStatus.Active

    property string journalName: ""
    property variant modelType
    property url userPicUrl

    BusyIndicator {
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        running: mnemosyManager.busy
        visible: running
    }

    Connections {
        target: mnemosyManager
        onInvalidUserName: {
            pageStack.pop()
        }
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

    function load() {
        mnemosyManager.loadUserJournal(journalName, modelType)
    }

    function getModel() {
        return modelType === Mnemosy.MyModel ?
                mnemosyManager.myJournalModel :
                mnemosyManager.userJournalModel
    }

    Component.onDestruction: {
        mnemosyManager.abortRequest()
    }

    SilicaListView {
        id: userJournalView

        anchors.fill: parent

        header: PageHeader {
            title: journalName
        }

        ViewPlaceholder {
            id: placeHolder
            enabled: !mnemosyManager.busy && !userJournalView.count
            text: qsTr("There are no entries.\nPull down to refresh.")
        }

        PullDownMenu {
            visible: mnemosyManager.logged

            MenuItem {
                visible: modelType !== Mnemosy.MyModel
                text: qsTr("Send message")
                onClicked: {
                    var dialog = pageStack.push("../dialogs/NewMessageDialog.qml")
                    dialog.accepted.connect (function () {
                        mnemosyManager.sendMessage(journalName,
                                dialog.subject, dialog.body)
                    })
                }
            }

            MenuItem {
                visible: modelType !== Mnemosy.MyModel
                text: mnemosyManager.isMyFriend(journalName) ?
                        qsTr("Remove friend") :
                        qsTr("Add as a friend")
                onClicked: {
                    var dialog = pageStack.push("../dialogs/AddEditFriendDialog.qml",
                            { friendName: journalName })
                    dialog.accepted.connect (function () {
                        mnemosyManager.addFriend(dialog.friendName,
                                dialog.groupMask)
                    })
                }
            }

            MenuItem {
                visible: modelType === Mnemosy.MyModel

                text: qsTr("New entry")
                onClicked: {
                    pageStack.push("../pages/AddEditEventPage.qml")
                }
            }

            MenuItem {
                text: qsTr("Refresh")
                onClicked: {
                    load()
                }
            }
        }

        model: getModel()
        function fetchMoreIfNeeded() {
            if (!userJournalPage.busy &&
                    indexAt(contentX, contentY + height) > getModel().rowCount() - 2) {
                mnemosyManager.loadNextUserJournalPage(journalName, modelType)
            }
        }
        onContentYChanged: fetchMoreIfNeeded()
        cacheBuffer: userJournalPage.height

        spacing: Theme.paddingSmall

        delegate: ListItem {
            id: listItem

            width: userJournalView.width
            contentHeight: column.height + Theme.paddingSmall

            clip: true

            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Copy url to clipboard")
                    onClicked: {
                        Clipboard.text = Utils.getLJEntryLink(journalName,
                            getModel().get(index).dItemId)
                    }
                }

                MenuItem {
                    visible: modelType === Mnemosy.MyModel
                    text: qsTr("Edit")
                    onClicked: {
                        pageStack.push("../pages/AddEditEventPage.qml",
                                { type: "edit", event: getModel().get(index),
                                    modelType: modelType, journalName: journalName})
                    }
                }

                MenuItem {
                    visible: modelType === Mnemosy.MyModel
                    text: qsTr("Delete")
                    onClicked: {
                        remove()
                    }
                }
            }

            property string _style: "<style>" +
                    "a:link { color:" + Theme.highlightColor + "; }" +
                    "p { color:" + Theme.primaryColor + "; }" +
                    "</style>"

            Column {
                id: column

                spacing: Theme.paddingSmall

                anchors.left: parent.left
                anchors.leftMargin: Theme.horizontalPageMargin
                anchors.right: parent.right
                anchors.rightMargin: Theme.horizontalPageMargin

                EntryHeaderItem {
                    width: parent.width

                    posterAvatar: userPicUrl
                    posterName: entryPosterName === "" ?
                            journalName.toUpperCase() :
                            entryPosterName.toUpperCase()
                    postDate: Utils.generateDateString(entryPostDate, "dd MMM yyyy hh:mm")
                    highlighted: listItem.highlighted || down
                }

                Label {
                    id: subjectLabel

                    width: parent.width

                    wrapMode: Text.WordWrap

                    font.pixelSize: Theme.fontSizeMedium
                    font.family: Theme.fontFamilyHeading
                    font.bold: true

                    style: Text.RichText

                    text: entrySubject
                    color: listItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                }

                Row {
                    width: parent.width
                    spacing: Theme.paddingMedium
                    visible: entryJournalType == Mnemosy.CommunityJournal

                    Label {
                        id: inCommunityLabel

                        font.pixelSize: Theme.fontSizeTiny
                        text: qsTr("Published in")
                        color: listItem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    }

                    ClickableLabel {
                        id: inCommunityLabelClickable

                        label.font.pixelSize: Theme.fontSizeTiny
                        label.text: entryJournalName.toUpperCase()
                        label.horizontalAlignment: Qt.AlignLeft
                        onClicked: {
                            var page = pageStack.push(Qt.resolvedUrl("UserJournalPage.qml"),
                                    { journalName: entryJournalName,
                                        modelType: Mnemosy.UserModel })
                            page.load()
                        }
                    }
                }

                Label {
                    id: entryText

                    width: parent.width

                    wrapMode: Text.WordWrap
                    textFormat: Text.RichText
                    horizontalAlignment: Qt.AlignJustify

                    font.pixelSize: Theme.fontSizeSmall
                    text: _style + (entryHasArg ?
                            entryEntryText.arg(parent.width) :
                            entryEntryText)
                    color: listItem.highlighted ? Theme.highlightColor : Theme.primaryColor

                    onLinkActivated: {
                        var journalName = Utils.getLJUserFromLink(link)
                        if (journalName !== undefined) {
                            var page = pageStack.push(Qt.resolvedUrl("UserJournalPage.qml"),
                                    { journalName: journalName,
                                        modelType: Mnemosy.UserModel })
                            page.load()
                            return
                        }

                        var pair = Utils.getLJEntryFromLink(link)
                        if (pair[0] !== undefined && pair[1] !== undefined &&
                                pair[1] !== entryDItemID) {
                            pageStack.push(Qt.resolvedUrl("EventPage.qml"),
                                    { dItemId: pair[1],
                                      modelType: Mnemosy.FeedModel,
                                      journalName: pair[0] })
                            return
                        }

                        Qt.openUrlExternally(link)
                    }
                }

                Label {
                    id: tagsLabel

                    width: parent.width

                    font.pixelSize: Theme.fontSizeTiny
                    font.italic: true

                    wrapMode: Text.WordWrap

                    visible: entryTags.length > 0

                    text: qsTr("Tags: ") + entryTags

                    color: listItem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                }

                Item {
                    height: childrenRect.height
                    width: parent.width

                    IconTextButton {
                        width: parent.width / 2

                        anchors.left: parent.left
                        anchors.right: parent.horizontalCenter

                        text: qsTr("%L1").arg(entryReplyCount)
                        icon: "image://theme/icon-m-chat?" + (down ?
                                Theme.highlightColor :
                                Theme.primaryColor)

                        onClicked: {
                            var page = pageStack.push("CommentsPage.qml",
                                    { "dItemId": entryDItemID,
                                        "journal": journalName })
                            page.load()
                        }
                    }

                    IconTextButton {
                        width: parent.width / 2

                        anchors.left: parent.horizontalCenter
                        anchors.right: parent.right

                        text: qsTr("Add comment")
                        icon: "image://theme/icon-m-message?" + (down ?
                                Theme.highlightColor :
                                Theme.primaryColor)
                        enabled: entryCanComment

                        onClicked: {
                            var dialog = pageStack.push("../dialogs/AddEditCommentDialog.qml")
                            dialog.accepted.connect(function () {
                                mnemosyManager.addComment(journalName,
                                        0, entryDItemID,
                                        dialog.subject, dialog.body, dialog.avatarId)
                            })
                        }
                    }
                }
            }

            onClicked: {
                pageStack.push(Qt.resolvedUrl("../pages/EventPage.qml"),
                        { event: getModel().get(index),
                          dItemId: getModel().get(index).dItemId,
                          modelType: modelType,
                          journalName: journalName,
                          userPicUrl: userPicUrl })
            }

            function remove() {
                remorse.execute(qsTr("Delete"),
                        function() {
                            mnemosyManager.deleteEvent(entryItemID, entryJournalName)
                        })
            }
            RemorsePopup { id: remorse }
        }

        VerticalScrollDecorator {}
    }
}

