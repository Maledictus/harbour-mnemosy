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
        if (status == PageStatus.Active &&
                pageStack._currentContainer.attachedContainer === null &&
                mnemosyManager.logged) {
            pageStack.pushAttached(Qt.resolvedUrl("ProfilePage.qml"))
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
        anchors.leftMargin: Theme.paddingSmall
        anchors.rightMargin: Theme.paddingSmall

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
                enabled: mnemosyManager.friendsPageModel.count
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
        }

        model: mnemosyManager.friendsPageModel

        spacing: Theme.paddingSmall

        delegate: ListItem {
            id: listItem

            width: parent.width
            contentHeight: contentItem.childrenRect.height + Theme.paddingSmall

            clip: true

            property string _style: "<style>" +
                    "a:link { color:" + Theme.highlightColor + "; }" +
                    "p { color:" + Theme.primaryColor + "; }" +
                    "</style>"

            Column {
                spacing: Theme.paddingSmall

                width: parent.width

                EntryHeaderItem {
                    width: parent.width

                    posterAvatar: entryPosterPicUrl
                    posterName: entryPosterName.toUpperCase()
                    postDate: Utils.generateDateString(entryPostDate)
                }

                Label {
                    id: subjectLabel

                    width: parent.width

                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                    font.pixelSize: Theme.fontSizeMedium
                    font.family: Theme.fontFamilyHeading
                    font.bold: true

                    style: Text.RichText

                    text: entrySubject
                }

                Label {
                    id: entryText

                    width: parent.width

                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    textFormat: Text.RichText

                    font.pixelSize: Theme.fontSizeSmall
                    text: _style + (entryHasArg ?
                            entryEntryText.arg(mainWindow.orientation == Orientation.Portrait ?
                                    Screen.width - 2 * Theme.paddingSmall :
                                    Screen.height - 2 * Theme.paddingSmall) :
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


