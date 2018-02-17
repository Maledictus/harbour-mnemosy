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

Dialog {
    id: deleteCommentDialog

    property string posterName
    property int deleteMask: 1
    Column {
        id: deleteColumn

        clip: true

        anchors.fill: parent

        spacing: Theme.paddingMedium

        DialogHeader {
            acceptText: qsTr("Delete")
            cancelText: qsTr("Cancel")
        }

        TextSwitch {
            id: deleteThisComment

            checked: true
            automaticCheck: false
            text: qsTr("Delete this comment")
        }

        TextSwitch {
            id: deleteThread

            checked: false
            text: qsTr("Delete thread (all subcomments)")

            onClicked: {
                if (checked) {
                    deleteMask = deleteMask | Mnemosy.DeleteThread
                }
                else {
                    deleteMask = deleteMask & ~Mnemosy.DeleteThread
                }
            }
        }

        TextSwitch {
            id: deleteAllComments

            checked: false
            text: qsTr("Delete all <b>%1</b> comments in this post").arg(posterName)

            onClicked: {
                if (checked) {
                    deleteMask |= Mnemosy.DeleteAllComment
                }
                else {
                    deleteMask &= ~Mnemosy.DeleteAllComment
                }
            }
        }
    }

    canAccept: deleteMask > 0
}
