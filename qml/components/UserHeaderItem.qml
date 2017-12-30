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
import QtQuick.Layouts 1.1
import Sailfish.Silica 1.0

Rectangle {
    property int userId : mnemosyManager.profile ?
            mnemosyManager.profile.userId :
            0

    height: Theme.itemSizeLarge + 2 * Theme.paddingMedium
    width: parent.width
    color: Theme.rgba(Theme.highlightBackgroundColor,
            Theme.highlightBackgroundOpacity)
    clip: true

    Image {
        id: profileAvatar

        anchors.left: parent.left
        anchors.leftMargin: Theme.paddingSmall

        anchors.verticalCenter: parent.verticalCenter

        width: Theme.itemSizeLarge
        height: Theme.itemSizeLarge

        sourceSize.width: width
        sourceSize.height: height

        source: mnemosyManager.profile && mnemosyManager.profile.avatarUrl !== "" ?
                mnemosyManager.profile.avatarUrl :
                "qrc:/images/blank_boy.png"
        onStatusChanged: {
            if (status === Image.Error) {
                source = "qrc:/images/blank_boy.png"
            }
        }
    }

    Label {
        id: fullNameLabel

        anchors.left: profileAvatar.right
        anchors.leftMargin: Theme.paddingMedium
        anchors.right: parent.right
        anchors.rightMargin: Theme.paddingMedium
        anchors.top: parent.top
        anchors.topMargin: Theme.paddingSmall

        color: Theme.primaryColor

        maximumLineCount: 2
        elide: Text.ElideRight
        wrapMode: Text.WordWrap

        text: mnemosyManager.profile ? mnemosyManager.profile.fullName : ""
    }

    RowLayout {
        anchors.left: profileAvatar.right
        anchors.leftMargin: Theme.paddingMedium
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.paddingSmall
        anchors.right: parent.right
        anchors.rightMargin: Theme.paddingMedium

        Label {
            id: userNameLabel

            Layout.alignment: Qt.AlignLeft

            color: Theme.primaryColor

            text: mnemosyManager.profile ? mnemosyManager.profile.userName : ""
        }

        Label {
            id: birthdayLabel

            Layout.alignment: Qt.AlignRight

            color: Theme.primaryColor

            text: mnemosyManager.profile ? mnemosyManager.profile.birthday : ""
        }
    }
}
