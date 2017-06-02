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

import QtQuick 2.5
import Sailfish.Silica 1.0
import harbour.mnemosy 1.0

Page {
    id: newEventPage

    property alias subject: subjectField.text
    property alias tags: tagsField.text
    property alias body: bodyArea.text
    property variant settings

    property string type: "add"
    property variant event
    property variant modelType
    property string journalName

    function attachPage() {
        if (pageStack._currentContainer.attachedContainer === null &&
                mnemosyManager.logged) {
            settings = pageStack.pushAttached(Qt.resolvedUrl("../pages/EventSettingsPage.qml"))
            if (type === "edit") {
                settings.journalName = journalName
                settings.event = event
                subject = event.subject
                body = event.event
                tags = event.tags
            }
        }
    }

    onStatusChanged: {
        if (status == PageStatus.Active) {
            attachPage()
        }
    }
    SilicaFlickable {
        id: postView

        anchors.fill: parent
        contentHeight: column.height + Theme.paddingSmall
        clip: true

        PullDownMenu {
            MenuItem {
                text: type === "add" ? qsTr("Submit") : qsTr("Update")
                enabled: bodyArea.text !== ""
                onClicked: {
                    if (type === "add") {
                        mnemosyManager.postEvent(subject, body, tags, settings.commentsEnabled,
                                settings.notificationsEnabled, settings.target, settings.adult,
                                settings.screening, settings.security, settings.groupMask);
                    }
                    else if (type === "edit") {
                        mnemosyManager.editEvent(event.itemId, subject, body, tags, settings.commentsEnabled,
                                settings.notificationsEnabled, settings.target, settings.adult,
                                settings.screening, settings.security, settings.groupMask);
                    }
                    pageStack.pop()
                }
            }
        }

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: type === "add" ? qsTr("New post") : qsTr("Edit post")
            }

            TextField {
                id: subjectField
                width: parent.width
                placeholderText: qsTr("Subject")
            }

            TextField {
                id: tagsField
                width: parent.width
                placeholderText: qsTr("Tags(comma separated)")
            }

            TextArea {
                id: bodyArea
                width: parent.width
                placeholderText: qsTr("Entry")
            }
        }
    }
}
