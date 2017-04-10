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
import Sailfish.Silica 1.0
import harbour.mnemosy 1.0

import "../components"
import "../utils/Utils.js" as Utils

Page {
    id: entryPage

    property variant event
    property int dItemId
    property string journalName
    property variant modelType: Mnemosy.FeedModel
    property url userPicUrl

    function attachPage() {
        if (pageStack._currentContainer.attachedContainer === null &&
                mnemosyManager.logged) {
            pageStack.pushAttached(Qt.resolvedUrl("ProfilePage.qml"))
        }
    }

    function getJournalName() {
        return journalName.length > 0 ? journalName : event.posterName
    }

    onStatusChanged: {
        if (status == PageStatus.Active) {
            mnemosyManager.abortRequest()
            attachPage()

            if (!event || event.fullEvent === "") {
                mnemosyManager.getEvent(dItemId, getJournalName(), modelType)
            }
        }
    }

    Connections {
        target: mnemosyManager
        onGotEvent: {
            event = newEvent
        }
    }

    SilicaFlickable {
        id: eventView

        anchors.fill: parent

        contentWidth: width
        contentHeight: column.height + Theme.paddingSmall

        clip: true

        PushUpMenu {
            visible: event ? event.fullEvent.length > 0 : false

            MenuItem {
                text: qsTr("Add comment")
                onClicked: {
                    var dialog = pageStack.push("../dialogs/AddCommentDialog.qml")
                    dialog.accepted.connect(function () {
                        mnemosyManager.addComment(getJournalName(),
                                0, event.dItemId,
                                dialog.subject, dialog.body)
                    })
                }
            }

            MenuItem {
                text: qsTr("Comments")
                onClicked: {
                    var page = pageStack.push("CommentsPage.qml",
                            { "dItemId": event.dItemId,
                                "journal": getJournalName() })
                    page.load()
                }
            }
        }

        property string _style: "<style>" +
                "a:link { color:" + Theme.highlightColor + "; }" +
                "p { color:" + Theme.primaryColor + "; }" +
                "</style>"

        Column {
            id: column

            spacing: Theme.paddingSmall

            width: eventView.width

            anchors.top: parent.top
            anchors.topMargin: Theme.paddingSmall
            anchors.left: parent.left
            anchors.leftMargin: Theme.horizontalPageMargin
            anchors.right: parent.right
            anchors.rightMargin: Theme.horizontalPageMargin

            EntryHeaderItem {
                width: parent.width

                posterAvatar: userPicUrl
                posterName: event ? event.posterName.toUpperCase() : ""
                postDate: event ?
                    Utils.generateDateString(event.postDate, "dd MMM yyyy hh:mm") :
                    ""
            }

            Label {
                id: tagsLabel

                width: parent.width

                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeTiny
                font.italic: true

                wrapMode: Text.WordWrap

                visible: event ? event.tags.length > 0 : ""

                text: qsTr("Tags: ") + (event ? event.tags : "")
            }

            Label {
                id: subjectLabel

                width: parent.width

                wrapMode: Text.WordWrap

                font.pixelSize: Theme.fontSizeMedium
                font.family: Theme.fontFamilyHeading
                font.bold: true

                style: Text.RichText

                text: event && event.subject.length > 0 ?
                        event.subject :
                        qsTr("Without subject")
            }

            Label {
                id: entryText

                width: parent.width

                wrapMode: Text.WordWrap
                textFormat: Text.RichText
                horizontalAlignment: Qt.AlignJustify

                font.pixelSize: Theme.fontSizeSmall
                text: eventView._style + (event && event.fullHasArg ?
                        event.fullEvent.arg(eventView.width -
                               2 * Theme.horizontalPageMargin) :
                        event ? event.fullEvent : "")

                onLinkActivated: {
                    Qt.openUrlExternally(link)
                }
            }
        }

        VerticalScrollDecorator {}
    }

    BusyIndicator {
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        running: mnemosyManager.busy
        visible: running
    }

}
