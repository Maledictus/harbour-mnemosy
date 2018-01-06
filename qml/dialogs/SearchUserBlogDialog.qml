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

Dialog {
    id: addFriendDialog

    property alias userName: searchField.text

    Column {
        id: column

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        clip: true

        DialogHeader {
            acceptText: qsTr("Search")
            cancelText: qsTr("Cancel")
        }

        SearchField {
            id: searchField
            width: parent.width
            placeholderText: qsTr("User name")

            onTextChanged: {
                listModel.update()
            }
        }
    }

    SilicaListView {
        width: parent.width

        anchors.left: parent.left
        anchors.leftMargin: Theme.horizontalPageMargin
        anchors.right: parent.right
        anchors.rightMargin: Theme.horizontalPageMargin
        anchors.top: column.bottom
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.paddingMedium

        currentIndex: -1

        clip: true

        model: ListModel {
            id: listModel
            property var users: mnemosyManager.getSearchedUsers()

            function update() {
                clear()
                for (var i=0; i<users.length; i++) {
                    if (searchField.text.length > 0 && users[i].indexOf(searchField.text) >= 0) {
                        append({"name": users[i]})
                    }
                }
            }

            Component.onCompleted: update()
        }

        delegate: ListItem {
            Label {
                anchors {
                    left: parent.left
                    leftMargin: searchField.textLeftMargin
                    verticalCenter: parent.verticalCenter
                }
                text: name
            }

            onClicked: {
                searchField.text = name
                listModel.clear()
            }
        }

        VerticalScrollDecorator {}
    }

    canAccept: searchField.text !== ""
}
