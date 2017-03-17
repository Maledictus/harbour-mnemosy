/*
The MIT License(MIT)

Copyright(c) 2016-2017 Oleg Linkin <maledictusdemagog@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
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
    id: filterFriendsPageDialog

    property int groupMask: 0

    SilicaListView {
        id: groupsView

        anchors.fill: parent

        header: DialogHeader {
            acceptText: qsTr("Accept filter")
            cancelText: qsTr("Cancel")
        }

        ViewPlaceholder {
            enabled: !mnemosyManager.busy &&
                    mnemosyManager.groupsModel.count === 0
            text: qsTr("There are no friends groups")
        }

        spacing: Theme.paddingSmall

        model: mnemosyManager.groupsModel

        delegate: ListItem {
            id: rootDelegateItem

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

    canAccept: !mnemosyManager.busy && mnemosyManager.groupsModel.count !== 0
}
