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

Page {
    property var _callback
    property var parentPage

    Component.onDestruction: {
        if (_callback) {
            _callback()
        }
    }

    SilicaGridView {
        id: grid
        header: PageHeader {
            title: qsTr("Avatars")
        }

        cellWidth: width / 3
        cellHeight: width / 3

        anchors.fill: parent

        model: mnemosyManager.userProfile.avatars

        delegate: Image {
            id: avatar
            asynchronous: true

            source: modelData.avatarUrl

            sourceSize.width: grid.cellWidth
            sourceSize.height: grid.cellHeight
            width: grid.cellWidth
            height: grid.cellHeight

            z: 1

            BusyIndicator {
                anchors.centerIn: parent
                visible: running
                running: avatar.status == Image.Loading
            }

            MouseArea {
                anchors.fill: parent
                onPressed: {
                    highlighter.z = 2
                }

                onClicked: {
                    function closure(avatarId, avatarUrl)
                    {
                        return function()
                        {
                            parentPage.avatarId = avatarId;
                            parentPage.avatar = avatarUrl;
                        }
                    }

                    _callback = closure(modelData.avatarId, modelData.avatarUrl);
                    pageStack.pop();
                }
            }

            Rectangle {
                id: highlighter

                opacity: 0.5
                color: Theme.highlightColor

                width: grid.cellWidth
                height: grid.cellHeight

                z: -1
            }
        }
        ScrollDecorator {}
    }
}
