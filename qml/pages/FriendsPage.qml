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

import "../components"
import "../utils/Utils.js" as Utils

Page {
    id: friendsPage

    function onSuccessLogin() {
        attachPage()
        load()
    }

    function attachPage() {
        if (pageStack._currentContainer.attachedContainer === null &&
                mnemosyManager.logged) {
            pageStack.pushAttached(Qt.resolvedUrl("ProfilePage.qml"))
        }
    }

    function load() {
        if (mnemosyManager.logged) {
            mnemosyManager.getFriendsPage()
        }
    }

    onStatusChanged: {
        if (status == PageStatus.Active && mnemosyManager.logged) {
            attachPage()
        }
    }

    BusyIndicator {
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        running: mnemosyManager.busy
        visible: running
    }

    SilicaListView {
        id: friendsPageView

        anchors.fill: parent

        header: PageHeader {
            title: qsTr("Friends Feed")
        }

        ViewPlaceholder {
            enabled: (!mnemosyManager.busy && !mnemosyManager.logged) ||
                    (!mnemosyManager.busy && !friendsPageView.count)
            text: {
                if (!mnemosyManager.busy && !mnemosyManager.logged) {
                    return qsTr("Authentification failed")
                }
                else if (!mnemosyManager.busy && !friendsPageView.count) {
                    return qsTr("There are no entries. Pull down to refresh.")
                }
                else {
                    return ""
                }
            }
        }

        PullDownMenu {
            visible: mnemosyManager.logged

            MenuItem {
                text: qsTr("Search user blog")
                onClicked: {
                    var dialog = pageStack.push("../dialogs/SearchUserBlogDialog.qml")
                    dialog.accepted.connect(function () {
                        mnemosyManager.saveSearchedUser(dialog.userName)
                        var page = pageStack.push(Qt.resolvedUrl("UserJournalPage.qml"),
                                { journalName: dialog.userName,
                                    modelType: Mnemosy.UserModel })
                        page.load()
                    })
                }
            }

            MenuItem {
                text: qsTr("Filter")
                onClicked: {
                    var dialog = pageStack.push("../dialogs/FilterFriendsPageDialog.qml",
                            { groupMask: applicationSettings.value("friendsPageFilter", 0) })
                    dialog.accepted.connect(function () {
                        applicationSettings.setValue("friendsPageFilter",
                                dialog.groupMask)
                        mnemosyManager.getFriendsPage()
                    })
                }
            }

            MenuItem {
                text: qsTr("Refresh")
                onClicked: {
                    mnemosyManager.getFriendsPage()
                }
            }

            MenuItem {
                text: qsTr("New post")
                onClicked: {
                    pageStack.push("../pages/AddEditEventPage.qml")
                }
            }
        }

        PushUpMenu {
            MenuItem {
                text: qsTr ("Load More...")
                visible: friendsPageView.count
                onClicked: {
                    mnemosyManager.getNextFriendsPage()
                }
            }

            MenuItem {
                text: qsTr("Go to top")
                onClicked: {
                    friendsPageView.scrollToTop()
                }
            }
            visible: mnemosyManager.logged && !mnemosyManager.busy &&
                    friendsPageView.count
        }

        model: mnemosyManager.friendsPageModel

        spacing: Theme.paddingSmall

        delegate: ListItem {
            id: listItem

            width: friendsPageView.width
            contentHeight: column.height + Theme.paddingSmall

            clip: true

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

                    posterAvatar: entryPosterPicUrl
                    posterName: entryPosterName.toUpperCase()
                    postDate: Utils.generateDateString(entryPostDate, "dd MMM yyyy hh:mm")

                    onClicked: {
                        var page = pageStack.push(Qt.resolvedUrl("UserJournalPage.qml"),
                                { journalName: entryPosterName,
                                    modelType: Mnemosy.UserModel,
                                    userPicUrl: entryPosterPicUrl })
                        page.load()
                    }
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
                }

                Row {
                    width: parent.width
                    spacing: Theme.paddingMedium
                    visible: entryJournalType == Mnemosy.CommunityJournal

                    Label {
                        id: inCommunityLabel

                        font.pixelSize: Theme.fontSizeTiny
                        text: qsTr("Posted in")
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

                    anchors.left: parent.left
                    anchors.right: parent.right

                    wrapMode: Text.WordWrap
                    textFormat: Text.RichText

                    horizontalAlignment: Text.AlignJustify

                    font.pixelSize: Theme.fontSizeSmall
                    text: _style + (entryHasArg ?
                            entryEntryText.arg(width) :
                            entryEntryText)
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
                        if (pair[0] !== undefined && pair[1] !== undefined) {
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

                    color: Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeTiny
                    font.italic: true

                    wrapMode: Text.WordWrap

                    visible: entryTags.length > 0

                    text: qsTr("Tags: ") + entryTags
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
                                        "journal": entryJournalName })
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
                                mnemosyManager.addComment(entryJournalName,
                                        0, entryDItemID,
                                        dialog.subject, dialog.body)
                            })
                        }
                    }
                }
            }

            onClicked: {
                pageStack.push(Qt.resolvedUrl("EventPage.qml"),
                        { event: mnemosyManager.friendsPageModel
                                .get(index),
                          dItemId: mnemosyManager.friendsPageModel
                                   .get(index).dItemId,
                          modelType: Mnemosy.FeedModel,
                          journalName: entryJournalName,
                          userPicUrl: entryPosterPicUrl })
            }
        }

        VerticalScrollDecorator{}
    }
}


