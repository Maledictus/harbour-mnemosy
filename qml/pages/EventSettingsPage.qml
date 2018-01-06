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

Page {
    id: eventSettingPage

    property alias commentsEnabled: commentsSwitch.checked
    property alias notificationsEnabled: notifySwitch.checked
    property alias target: targetComboBox.value
    property int adult: Mnemosy.WithoutAdult
    property int screening: Mnemosy.NoScreening
    property int security: Mnemosy.PublicAccess
    property int groupMask: 0

    property variant event
    property variant journalName

    onEventChanged: {
        targetComboBox.value = journalName
        commentsSwitch.checked = event.properties.commentsEnabled
        notifySwitch.checked = event.properties.notifyByEmail
        adultComboxBox.currentIndex = event.properties.adult
        screeningComboxBox.currentIndex = event.properties.screening
        if (event.access === Mnemosy.CustomAccess && !event.groupMask) {
            securityComboxBox.currentIndex = Mnemosy.FriendsOnlyAccess
        }
        else {
            securityComboxBox.currentIndex = event.access
        }
        groupMask = event.groupMask
    }

    function loadFromDefault() {
        targetComboBox.value = applicationSettings.value('eventTarget',
                mnemosyManager.userProfile.userName)
        commentsSwitch.checked = applicationSettings.value('eventCommentsEnabled', true)
        notifySwitch.checked = applicationSettings.value('eventNotifyByEmail', true)
        adultComboxBox.currentIndex =applicationSettings.value('eventAdult', Mnemosy.WithoutAdult)
        screeningComboxBox.currentIndex = applicationSettings.value('eventScreening', Mnemosy.NoScreening)
        var secur = applicationSettings.value('eventSecurity', Mnemosy.PublicAccess)
        groupMask = applicationSettings.value('eventGroupMask', 0)
        if (secur === Mnemosy.CustomAccess && !groupMask) {
            securityComboxBox.currentIndex = Mnemosy.FriendsOnlyAccess
        }
        else {
            securityComboxBox.currentIndex = secur
        }
    }

    Component.onCompleted: {
        loadFromDefault()
    }

    SilicaFlickable {
        id: settingsView

        anchors.fill: parent

        contentWidth: width
        contentHeight: column.height + Theme.paddingSmall

        clip: true

        PullDownMenu {
            MenuItem {
                text: qsTr("Save settings")
                onClicked: {
                    applicationSettings.setValue('eventTarget', target)
                    applicationSettings.setValue('eventCommentsEnabled', commentsEnabled)
                    applicationSettings.setValue('eventNotifyByEmail', notificationsEnabled)
                    applicationSettings.setValue('eventAdult', adult)
                    applicationSettings.setValue('eventScreening', screening)
                    applicationSettings.setValue('eventSecurity', security)
                    applicationSettings.setValue('eventGroupMask', groupMask)
                }
            }
        }

        Column {
            id: column

            spacing: Theme.paddingSmall

            anchors.left: parent.left
            anchors.leftMargin: Theme.horizontalPageMargin
            anchors.right: parent.right
            anchors.rightMargin: Theme.horizontalPageMargin

            PageHeader {
                title: qsTr("Settings")
            }

            ComboBox {
                id: targetComboBox

                width: parent.width

                label: qsTr("Publish to")

                menu: ContextMenu {
                    id: contexMenu
                    Repeater {
                        model: mnemosyManager.getAvailablePostTargets()
                        delegate: MenuItem {
                            text: modelData
                        }
                    }
                }
            }

            TextSwitch {
                id: commentsSwitch

                width: parent.width

                text: qsTr("Comments")
                checked: true
            }

            TextSwitch {
                id: notifySwitch

                width: parent.width

                text: qsTr("Notify by email")
                checked: true
            }

            ComboBox {
                id: adultComboxBox

                width: parent.width

                label: qsTr("Adult content")
                currentIndex: 0
                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("none")
                        onClicked: adult = Mnemosy.WithoutAdult
                    }
                    MenuItem {
                        text: qsTr("concepts")
                        onClicked: adult = Mnemosy.AdutlFrom14
                    }
                    MenuItem {
                        text: qsTr("explicit")
                        onClicked: adult = Mnemosy.AdultFrom18
                    }
                }
            }

            ComboBox {
                id: screeningComboxBox

                width: parent.width

                label: qsTr("Screening comments")
                currentIndex: 0
                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("none")
                        onClicked: screening = Mnemosy.NoScreening
                    }
                    MenuItem {
                        text: qsTr("non friends")
                        onClicked: screening = Mnemosy.ScreenNoFriends
                    }
                    MenuItem {
                        text: qsTr("all")
                        onClicked: screening = Mnemosy.ScreenAll
                    }
                    MenuItem {
                        text: qsTr("anonymous")
                        onClicked: screening = Mnemosy.ScreenAnonymous
                    }
                    MenuItem {
                        text: qsTr("with links")
                        onClicked: screening = Mnemosy.ScreenWithLinks
                    }
                }
            }

            ComboBox {
                id: securityComboxBox

                width: parent.width

                label: qsTr("Visibility")
                currentIndex: 0
                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("public")
                        onClicked: security  = Mnemosy.PublicAccess
                    }
                    MenuItem {
                        text: qsTr("friends")
                        onClicked: {
                            security  = Mnemosy.FriendsOnlyAccess
                            groupMask = 1
                        }
                    }
                    MenuItem {
                        text: qsTr("private")
                        onClicked: security  = Mnemosy.PrivateAccess
                    }
                    MenuItem {
                        text: qsTr("custom")
                        onClicked: {
                            security = Mnemosy.CustomAccess
                            var dialog = pageStack.push (Qt.resolvedUrl("../dialogs/FriendsGroupsSelectorDialog.qml"),
                                    { groupMask: eventSettingPage.groupMask });
                            dialog.accepted.connect(function () {
                                groupMask = dialog.groupMask
                            })
                        }
                    }
                }
            }
        }
    }
}
