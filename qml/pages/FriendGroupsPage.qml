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

Page {
    id: friendsGroupsPage

    function attachPage() {
        if (pageStack._currentContainer.attachedContainer === null &&
                mnemosyManager.logged) {
            pageStack.pushAttached(Qt.resolvedUrl("ProfilePage.qml"))
        }
    }

    onStatusChanged: {
        if (status == PageStatus.Active) {
            attachPage()
        }
    }

    SilicaListView {
        id: groupsView

        anchors.fill: parent

        header: PageHeader {
            title: qsTr("Friends groups")
        }

        ViewPlaceholder {
            enabled: !mnemosyManager.busy &&
                    groupsView.count === 0
            text: qsTr("There are no friends groups.\nPull down to add new one")
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Add group")
                onClicked: {
                    var dialog = pageStack.push("../dialogs/AddFriendGroupDialog.qml");
                    dialog.accepted.connect (function() {
                        mnemosyManager.addFriendGroup(dialog.groupName,
                                dialog.privateGroup)
                    })
                }
            }

            MenuItem {
                text: qsTr("Refresh")
                onClicked: {
                    mnemosyManager.getFriendGroups()
                }
            }
        }

        spacing: Theme.paddingSmall

        model: mnemosyManager.groupsModel

        delegate: ListItem {
            id: rootDelegateItem

            width: groupsView.width

            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Delete")
                    onClicked: {
                        remove()
                    }
                }
            }

            Label {
                id: groupNameLabel

                anchors.left: parent.left
                anchors.leftMargin: Theme.horizontalPageMargin
                anchors.right: privateImage.visible ?
                        privateImage.left :
                        parent.right
                anchors.rightMargin: privateImage.visible ?
                        Theme.paddingMedium :
                        Theme.horizontalPageMargin
                anchors.verticalCenter: parent.verticalCenter

                elide: Text.ElideRight
                text: groupName
            }

            Image {
                id: privateImage

                anchors.right: parent.right
                anchors.rightMargin: Theme.horizontalPageMargin
                anchors.verticalCenter: parent.verticalCenter

                visible: groupPublic === false
                source: "image://theme/icon-m-device-lock"
            }

            function remove() {
                remorse.execute(rootDelegateItem, qsTr("Delete"),
                        function() {
                            mnemosyManager.deleteFriendGroup(groupId)
                        })
            }
            RemorseItem { id: remorse }
        }

        VerticalScrollDecorator {}
    }

    BusyIndicator {
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        running: mnemosyManager.busy
        visible: running
    }
}
