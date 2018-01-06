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

CoverBackground {
    BackgroundItem {
        anchors.fill: parent

        Image {
            id: coverBgImage
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/mnemosy128x128.png"
            opacity: 0.2

            width: 128
            height: 128
            sourceSize.height: width
            sourceSize.width: height
        }
    }

    function loadModel () {
        listView.model = manager.bookmarksModel
    }

    SilicaListView {
        id: listView

        anchors.bottom: actionsList.top
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Theme.paddingMedium
        height: 240

        spacing: Theme.paddingMedium

        width: parent.width
        clip: true

        model: mnemosyManager.friendsPageModel

        delegate: BackgroundItem {
            id: delegate
            height: 30

            Label {
                id: titleLabel

                anchors.fill: parent

                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeTiny
                elide: Text.ElideRight

                text: entrySubject
            }
        }
    }

    CoverActionList {
        id: actionsList
        CoverAction {
            iconSource: "image://theme/icon-cover-refresh"
            onTriggered: mnemosyManager.getFriendsPage()
        }

        CoverAction {
            iconSource: "image://theme/icon-cover-new"
            onTriggered: {
                pageStack.push("../pages/AddEditEventPage.qml", {},PageStackAction.Immediate);
                mainWindow.activate()
            }
        }
    }
}


