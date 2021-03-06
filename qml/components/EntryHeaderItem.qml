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

MouseArea {
    id: entryHeaderItem

    height: Theme.itemSizeSmall

    property string posterAvatar
    property alias posterName: posterNameLabel.text
    property alias postDate: postDateLabel.text
    property string posterDefaultAvatar : "qrc:/images/blank_boy.png"

    property bool down: pressed && containsMouse
    property bool highlighted: down

    Image {
        id: posterAvatarImage

        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter

        width: Theme.iconSizeMedium
        height: Theme.iconSizeMedium

        sourceSize.width: width
        sourceSize.height: height
        source: posterAvatar.length > 0 ? posterAvatar : posterDefaultAvatar

        onStatusChanged: {
            if (status === Image.Error) {
                source = posterDefaultAvatar
            }
        }
    }

    Label {
        id: posterNameLabel

        anchors.left: posterAvatarImage.right
        anchors.leftMargin: Theme.paddingSmall
        anchors.right: postDateLabel.left
        anchors.rightMargin: Theme.paddingSmall
        anchors.verticalCenter: parent.verticalCenter

        font.pixelSize: Theme.fontSizeSmall
        font.bold: true
        elide: Text.ElideRight
        textFormat: Text.StyledText

        color: highlighted ? Theme.highlightColor : Theme.primaryColor
    }

    Label {
        id: postDateLabel

        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        font.pixelSize: Theme.fontSizeExtraSmall
        color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
    }
}
