TARGET = harbour-mnemosy

VERSION = 0.4.1

QT += xml xmlpatterns qml

CONFIG += link_pkgconfig sailfishapp c++11
PKGCONFIG += mlite5

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

RESOURCES += $${TARGET}.qrc

SOURCES += src/main.cpp \
    src/debugmessagehandler.cpp \
    src/application.cpp \
    src/settings/accountsettings.cpp \
    src/mnemosymanager.cpp \
    src/settings/applicationsettings.cpp \
    src/lj-rpc/ljxmlrpc.cpp \
    src/lj-rpc/rpcutils.cpp \
    src/lj-rpc/ljparsertype.cpp \
    src/userprofile.cpp \
    src/enumsproxy.cpp \
    src/ljevent.cpp \
    src/ljeventproperties.cpp \
    src/models/ljeventsmodel.cpp \
    src/ljcomment.cpp \
    src/models/ljcommentsmodel.cpp \
    src/ljcommentproperties.cpp \
    src/utils.cpp \
    src/models/ljfriendsgroupsmodel.cpp \
    src/ljfriendsgroup.cpp \
    src/models/friendssortfilterproxymodel.cpp \
    src/models/ljfriendsmodel.cpp \
    src/ljfriend.cpp

HEADERS += src/debugmessagehandler.h \
    src/application.h \
    src/settings/accountsettings.h \
    src/mnemosymanager.h \
    src/settings/applicationsettings.h \
    src/lj-rpc/ljxmlrpc.h \
    src/lj-rpc/rpcutils.h \
    src/lj-rpc/ljparsertype.h \
    src/userprofile.h \
    src/enumsproxy.h \
    src/ljevent.h \
    src/ljeventproperties.h \
    src/models/ljeventsmodel.h \
    src/ljcomment.h \
    src/models/ljcommentsmodel.h \
    src/ljcommentproperties.h \
    src/utils.h \
    src/models/ljfriendsgroupsmodel.h \
    src/ljfriendsgroup.h \
    src/models/friendssortfilterproxymodel.h \
    src/models/ljfriendsmodel.h \
    src/ljfriend.h

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

DISTFILES += \
    qml/pages/AuthorizationDialog.qml \
    qml/pages/ProfilePage.qml \
    qml/components/UserHeaderItem.qml \
    qml/pages/FriendsPage.qml \
    qml/components/EntryHeaderItem.qml \
    qml/utils/Utils.js \
    qml/pages/EventPage.qml \
    qml/components/IconTextButton.qml \
    qml/pages/AddCommentDialog.qml \
    qml/pages/CommentsPage.qml \
    qml/components/ClickableLabel.qml \
    qml/pages/SplashScreenPage.qml \
    qml/components/Popup.qml \
    qml/pages/FriendGroupsPage.qml \
    qml/dialogs/AddCommentDialog.qml \
    qml/dialogs/AuthorizationDialog.qml \
    qml/dialogs/AddFriendGroupDialog.qml \
    qml/pages/FriendGroupsPage.qml \
    rpm/harbour-mnemosy.changes \
    qml/cover/CoverPage.qml \
    harbour-mnemosy.desktop \
    rpm/harbour-mnemosy.spec\
    qml/harbour-mnemosy.qml \
    qml/pages/UserJournalPage.qml \
    qml/pages/FriendsListPage.qml \
    qml/utils/FriendsSorter.qml \
    qml/pages/SorterSelectorPage.qml \
    qml/dialogs/SorterSelectorDialog.qml \
    qml/dialogs/FilterFriendsPage.qml \
    qml/pages/FriendsSorterSelectorPage.qml \
    qml/dialogs/AddFriendDialog.qml \
    qml/dialogs/FilterFriendsPageDialog.qml

lupdate_only{
SOURCES = qml/pages/AuthorizationDialog.qml \
    qml/pages/ProfilePage.qml \
    qml/pages/FriendsPage.qml \
    qml/utils/Utils.js \
    qml/pages/EventPage.qml \
    qml/pages/AddCommentDialog.qml \
    qml/pages/CommentsPage.qml \
    qml/pages/FriendGroupsPage.qml \
    qml/dialogs/AddCommentDialog.qml \
    qml/dialogs/AuthorizationDialog.qml \
    qml/dialogs/AddFriendGroupDialog.qml \
    qml/pages/FriendGroupsPage.qml \
    qml/cover/CoverPage.qml \
    qml/pages/UserJournalPage.qml \
    qml/pages/FriendsListPage.qml \

}

#CONFIG += sailfishapp_i18n
#TRANSLATIONS += translations/harbour-mnemosy-ru.ts
