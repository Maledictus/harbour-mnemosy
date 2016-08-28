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

Dialog {
    id: addFriendDialog

    property alias friendName: friendNameField.text
    property alias fullName: fullNameLabel.text
    property string type: "add"
    property int groupMask: 0

    Column {
        id: column

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        clip: true

        DialogHeader {
            acceptText: type == "add" ? qsTr("Add friend") : qsTr("Edit friend")
            cancelText: qsTr("Cancel")
        }

        spacing: Theme.paddingSmall

        TextField {
            id: friendNameField

            anchors.left: parent.left
            anchors.right: parent.right

            placeholderText: qsTr ("Friend name...")

            visible: type == "add"
        }

        Label {
            id: friendNameLabel

            anchors.left: parent.left
            anchors.leftMargin: Theme.horizontalPageMargin
            anchors.right: parent.right
            anchors.rightMargin: Theme.horizontalPageMargin

            color: Theme.highlightColor

            visible: type != "add"

            text: friendName
        }

        Label {
            id: fullNameLabel

            anchors.left: parent.left
            anchors.leftMargin: Theme.horizontalPageMargin
            anchors.right: parent.right
            anchors.rightMargin: Theme.horizontalPageMargin

            visible: type != "add"
            font.italic: true
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.highlightColor
        }

        Label {
            anchors.left: parent.left
            anchors.leftMargin: Theme.horizontalPageMargin
            anchors.right: parent.right
            anchors.rightMargin: Theme.horizontalPageMargin

            color: Theme.highlightColor
            text: qsTr("Add to groups:")
        }
    }

    SilicaListView {
        id: groupsView

        clip: true

        anchors.left: parent.left
        anchors.leftMargin: Theme.horizontalPageMargin
        anchors.right: parent.right
        anchors.rightMargin: Theme.horizontalPageMargin
        anchors.top: column.bottom
        anchors.topMargin: Theme.paddingSmall
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.paddingSmall

        model: mnemosyManager.groupsModel

        delegate: ListItem {
            width: groupsView.width

            TextSwitch {
                id: groupSwitch
                text: groupName
                checked: groupMask & (1 << groupId)
                onClicked: {
                    if (checked) {
                        groupMask |= (1 << groupId)
                    }
                    else {
                        groupMask = groupMask & ~(1 << groupId)
                    }
                }
            }
        }

        VerticalScrollDecorator {}
    }

    canAccept: friendNameField.text !== ""
}
