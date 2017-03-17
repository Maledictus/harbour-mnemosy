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
import harbour.mnemosy 1.0

import "../utils"

Page {
    id: friendsListPage

    property string searchText

    property FriendsSorter friendsSorter: getSorterByKey(applicationSettings
            .value("friendsSort", "asc"))

    function getSorterByKey(key) {
        for (var i = 0; i < friendsSorters.length; ++i) {
            if (friendsSorters[i].key === key) {
                return friendsSorters[i]
            }
        }

        return azSorter;
    }


    property FriendsSorter azSorter: FriendsSorter {
        name: "a-z"
        key: "asc"
    }

    property FriendsSorter zaSorter: FriendsSorter {
        name: "z-a"
        key: "desc"
    }

    property FriendsSorter statusAscSorter: FriendsSorter {
        name: qsTr ("Friend's status (Mutual first)")
        key: "mutual_asc"
    }

    property FriendsSorter statusDescSorter: FriendsSorter {
        name: qsTr ("Friend's status (Mutual last)")
        key: "mutual_desc"
    }

    property variant friendsSorters: [
        azSorter,
        zaSorter,
        statusAscSorter,
        statusDescSorter
    ]

    onFriendsSorterChanged: {
        if (friendsSorter.key == "asc")
        {
            mnemosyManager.friendsModel.sortFriends(Mnemosy.TextSort,
                    Qt.AscendingOrder)
        }
        else if (friendsSorter.key == "desc")
        {
            mnemosyManager.friendsModel.sortFriends(Mnemosy.TextSort,
                    Qt.DescendingOrder)
        }
        else if (friendsSorter.key == "mutual_asc")
        {
            mnemosyManager.friendsModel.sortFriends(Mnemosy.StatusSort,
                    Qt.AscendingOrder)
        }
        else if (friendsSorter.key == "mutual_desc")
        {
            mnemosyManager.friendsModel.sortFriends(Mnemosy.StatusSort,
                    Qt.DescendingOrder)
        }

        applicationSettings.setValue("friendsSort", friendsSorter.key)
    }

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
        id: friendsListView

        anchors.fill: parent

        header: Column {
            id: headerColumn

            width: friendsListView.width

            spacing: 0

            property alias searchField: search

            PageHeader {
                title: qsTr("Friends")
            }

            SearchField {
                id: search

                width: parent.width
                placeholderText: qsTr("Search")

                onTextChanged: {
                    mnemosyManager.friendsModel.filterFriends(Mnemosy.Text, text)
                    search.forceActiveFocus ()
                    friendsListView.currentIndex = -1
                }
            }
        }

        ViewPlaceholder {
            enabled: !mnemosyManager.busy && !friendsListView.count &&
                     friendsListView.headerItem.searchField.text.length === 0
            text: qsTr("There are no friends. Pull down to refresh")
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Add friend")
                onClicked: {
                    var dialog = pageStack.push("../dialogs/AddFriendDialog.qml")
                    dialog.accepted.connect (function () {
                        mnemosyManager.addFriend(dialog.friendName,
                                dialog.groupMask)
                    })
                }
            }

            MenuItem {
                text: qsTr("Sort by: %1").arg(friendsSorter.name)
                onClicked: {
                    pageStack.push (Qt.resolvedUrl("FriendsSorterSelectorPage.qml"),
                            { parentPage: friendsListPage });
                }
            }

            MenuItem {
                text: qsTr("Refresh")
                onClicked: {
                    mnemosyManager.getFriends()
                }
            }
        }

        currentIndex: -1

        spacing: Theme.paddingMedium

        model: mnemosyManager.friendsModel

        delegate: ListItem {
            id: rootDelegateItem

            width: friendsListView.width

            menu: ContextMenu {
                MenuItem {
                    visible: !friendMy
                    text: qsTr("Add")
                    onClicked: {
                        var dialog = pageStack.push ("../dialogs/AddFriendDialog.qml",
                                { friendName: friendUserName,
                                    groupMask: friendGroupMask, type: "add" })
                        dialog.accepted.connect (function () {
                            mnemosyManager.addFriend(dialog.friendName,
                                    dialog.groupMask)
                        })
                    }
                }

                MenuItem {
                    visible: friendMy
                    text: qsTr("Edit")
                    onClicked: {
                        var dialog = pageStack.push ("../dialogs/AddFriendDialog.qml",
                                { friendName: friendUserName,
                                    groupMask: friendGroupMask, type: "edit",
                                    fullName: friendFullName })
                        dialog.accepted.connect (function () {
                            mnemosyManager.editFriend(dialog.friendName,
                                    dialog.groupMask)
                        })
                    }
                }
                MenuItem {
                    visible: friendMy
                    text: qsTr("Delete")
                    onClicked: {
                        remove()
                    }
                }

                MenuItem {
                    text: qsTr("Show user blog")
                    onClicked: {
                        var page = pageStack.push(Qt.resolvedUrl("UserJournalPage.qml"),
                                { journalName: friendUserName,
                                    modelType: Mnemosy.UserModel,
                                    userPicUrl: friendAvatar})
                        page.load()
                    }
                }

            }

            Image {
                id: avatarImage
                anchors.left: parent.left
                anchors.leftMargin: Theme.horizontalPageMargin
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: Theme.horizontalPageMargin

                sourceSize.height: 64
                sourceSize.width: 64

                source: friendAvatar

                onStatusChanged: {
                    if (status == Image.Error) {
                        source = "qrc:/images/blank_boy.png"
                    }
                }
            }

            Column
            {
                anchors.top: parent.top
                anchors.topMargin: Theme.paddingSmall
                anchors.left: avatarImage.right
                anchors.leftMargin: Theme.paddingMedium
                anchors.right: statusImage.left
                anchors.rightMargin: Theme.paddingMedium

                spacing: 0

                Label {
                    id: nameLabel

                    elide: Text.ElideRight
                    text: friendUserName
                }

                Label {
                    font.pixelSize: Theme.fontSizeTiny
                    font.italic: true

                    elide: Text.ElideRight
                    visible: friendFullName.length > 0
                    text: friendFullName
                }
            }

            Image {
                id: statusImage
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: Theme.horizontalPageMargin

                source: {
                    if (friendOfMe && friendMy) {
                        return "image://Theme/icon-status-data-traffic"
                    }
                    else if (friendOfMe) {
                        return "image://Theme/icon-status-data-upload"
                    }
                    else {
                        return "image://Theme/icon-status-data-download"
                    }
                }
            }

            function remove() {
                remorse.execute(rootDelegateItem, qsTr("Delete"),
                        function() {
                            mnemosyManager.deleteFriend(friendUserName)
                        })
            }
            RemorseItem { id: remorse }
        }

        VerticalScrollDecorator {}
    }
}
