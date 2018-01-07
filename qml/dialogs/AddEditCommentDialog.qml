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

import "../components"

Dialog {
    id: addCommentDialog

    property alias subject: subjectField.text
    property alias body: bodyArea.text
    property string type: "add"
    property string avatarId
    property alias avatar: avatarImage.source

    onAvatarIdChanged: {
        var avatarObject
        for (avatarObject in mnemosyManager.userProfile.avatars) {
            if (avatarObject.avatarId === avatarId) {
                avatarImage.source = avatarObject.avatarUrl
                break;
            }
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: replyColumn.height + Theme.paddingSmall

        Column {
            id: replyColumn

            clip: true
            width: parent.width

            spacing: Theme.paddingMedium

            DialogHeader {
                acceptText: {
                    if (type == "add") {
                        return qsTr("Add")
                    }
                    else if (type == "edit") {
                        return qsTr("Update")
                    }
                    return qsTr("Ok")
                }
                cancelText: qsTr("Cancel")
            }

            TextField {
                id: subjectField

                width: parent.width

                placeholderText: qsTr("Subject")

                EnterKey.enabled: text.length > 0
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: bodyArea.focus = true
            }

            TextArea {
                id: bodyArea

                width: parent.width

                placeholderText: qsTr("Comment")
            }

            UserAvatarItem {
                id: avatarImage

                width: parent.width

                visible: type !== "edit"

                source: mnemosyManager.userProfile.avatarUrl
                parentPage: addCommentDialog
            }
        }
    }
    canAccept: bodyArea.text !== ""
}
