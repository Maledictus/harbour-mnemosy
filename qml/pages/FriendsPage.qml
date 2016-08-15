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

import QtQuick 2.0
import Sailfish.Silica 1.0
import org.mnemosy 1.0

import "../components"
import "../utils/Utils.js" as Utils

Page {
    id: friendsPage

    onStatusChanged: {
        attachPage()
    }

    function attachPage() {
        if (status == PageStatus.Active) {
            mnemosyManager.abortRequest()

            if (pageStack._currentContainer.attachedContainer === null &&
                    mnemosyManager.logged) {
                pageStack.pushAttached(Qt.resolvedUrl("ProfilePage.qml"))
            }
        }
    }

    function load() {
        console.log("load")
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
                    (!mnemosyManager.busy &&
                            mnemosyManager.friendsPageModel.count === 0)
            text: {
                if (!mnemosyManager.busy && !mnemosyManager.logged) {
                    return qsTr("Authentification failed")
                }
                else if (!mnemosyManager.busy &&
                        mnemosyManager.friendsPageModel.count === 0) {
                    return qsTr("There are no entries. Pull down to refresh.")
                }
                else {
                    return ""
                }
            }
        }

        PullDownMenu {
            MenuItem {
                text: qsTr ("Refresh")
                onClicked: {
                    mnemosyManager.getFriendsPage()
                }
            }
        }

        PushUpMenu {
            MenuItem {
                text: qsTr ("Load More...")
                visible: mnemosyManager.friendsPageModel.count
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
            visible: !mnemosyManager.busy && mnemosyManager.friendsPageModel.count !== 0
        }

        model: mnemosyManager.friendsPageModel

        spacing: Theme.paddingSmall

        delegate: ListItem {
            id: listItem

            width: friendsPageView.width
            contentHeight: contentItem.childrenRect.height +
                    2 * Theme.paddingSmall

            clip: true

            property string _style: "<style>" +
                    "a:link { color:" + Theme.highlightColor + "; }" +
                    "p { color:" + Theme.primaryColor + "; }" +
                    "</style>"

            Column {
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

                    posterAvatar: entryPosterPicUrl
                    posterName: entryPosterName.toUpperCase()
                    postDate: Utils.generateDateString(entryPostDate)
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

                        font.pixelSize: Theme.fontSizeTiny
                        text: entryJournalName.toUpperCase()

                        onClicked: {
                            console.log("Show community")
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
                            entryEntryText.arg(friendsPageView.width - 2 * Theme.horizontalPageMargin) :
                            entryEntryText)
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

                        text: qsTr("%1").arg(entryReplyCount)
                        icon: "image://theme/icon-m-chat?" + (down ?
                                Theme.highlightColor :
                                Theme.primaryColor)

                        onClicked: {
                            pageStack.push("CommentsPage.qml",
                                    { "dItemId": entryDItemID,
                                            "journal": entryJournalName })
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
                            var dialog = pageStack.push("AddCommentDialog.qml")
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
                          modelType: Mnemosy.FeedModel })
            }
        }

        VerticalScrollDecorator{}
    }
}


