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

Page {
    id: friendsPage

    onStatusChanged: {
        attachPage()
    }

    function attachPage() {
        if (status == PageStatus.Active &&
                pageStack._currentContainer.attachedContainer === null &&
                mnemosyManager.logged) {
            pageStack.pushAttached(Qt.resolvedUrl("ProfilePage.qml"))
            console.log("page attached")
        }
    }

    function load() {
        console.log("load")
    }

    BusyIndicator {
        size: BusyIndicatorSize.Medium
        anchors.centerIn: parent
        running: mnemosyManager.busy
        visible: running
    }

    SilicaListView {
        id: friendsPageView

        anchors.fill: parent

        header: PageHeader {
            title: qsTr("Friends Feed")
        }

        ViewPlaceholder {
            enabled: (!mnemosyManager.busy && !mnemosyManager.logged) ||
                    (!mnemosyManager.busy &&
                            mnemosyManager.friendsPageModel.count === 0)
            text: {
                if (!mnemosyManager.busy && !mnemosyManager.logged) {
                    return qsTr("Authentification failed")
                }
                else if (!mnemosyManager.busy &&
                        mnemosyManager.friendsPageModel.count === 0) {
                    return qsTr("There are no entries. Pull down to refresh.")
                }
                else {
                    return ""
                }
            }
        }

        PullDownMenu {
            MenuItem {
                text: qsTr ("Refresh")
                onClicked: {
                    mnemosyManager.getFriendsPage()
                }
            }
        }

        model: mnemosyManager.friendsPageModel

        delegate: entryDelegate

        VerticalScrollDecorator{}
    }

    Component {
        id: entryDelegate

        ListItem {
            id: item
        }
    }
}

