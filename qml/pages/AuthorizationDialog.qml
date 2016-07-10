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
    id: authorizeDialog

    property alias login: loginField.text
    property alias password: passwordField.text

    canAccept: login.length > 0 && password.length > 0

    Column {
        width: parent.width

        DialogHeader {
            dialog: authorizeDialog

            acceptText: qsTr("Login")
            cancelText: qsTr("Cancel")
        }

        Image {
            id: ljLogo

            anchors.left: parent.left
            anchors.leftMargin: Theme.paddingMedium

            clip: true
            smooth: true
            asynchronous: true
            fillMode: Image.PreserveAspectFit
            sourceSize.width: 128
            sourceSize.height: 128

            source: "qrc:/images/livejournal.png"
        }

        TextField {
            id: loginField

            anchors.left: parent.left
            anchors.right: parent.right

            placeholderText: qsTr("Login")
        }

        TextField {
            id: passwordField

            anchors.left: parent.left
            anchors.right: parent.right

            placeholderText: qsTr("Password")
            echoMode: TextInput.Password
        }
    }
}
