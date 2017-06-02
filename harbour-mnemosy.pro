TARGET = harbour-mnemosy

VERSION = 0.6.1

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
    src/ljfriend.cpp \
    src/ljmessage.cpp \
    src/models/ljmessagesmodel.cpp \
    src/models/ljfriendsmodel.cpp \
    src/models/messagessortfilterproxymodel.cpp

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
    src/ljfriend.h \
    src/ljmessage.h \
    src/models/ljmessagesmodel.h \
    src/models/cachedmodel.h \
    src/models/ljfriendsmodel.h \
    src/models/messagessortfilterproxymodel.h

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

DISTFILES += \
    qml/components/UserHeaderItem.qml \
    qml/components/EntryHeaderItem.qml \
    qml/components/IconTextButton.qml \
    qml/components/ClickableLabel.qml \
    qml/components/Popup.qml \
    qml/cover/CoverPage.qml \
    qml/dialogs/AddEditCommentDialog.qml \
    qml/dialogs/AddEditFriendDialog.qml \
    qml/dialogs/AddFriendGroupDialog.qml \
    qml/dialogs/AuthorizationDialog.qml \
    qml/dialogs/DeleteCommentDialog.qml \
    qml/dialogs/FilterFriendsPageDialog.qml \
    qml/dialogs/FriendsGroupsSelectorDialog.qml \
    qml/dialogs/SearchUserBlogDialog.qml \
    qml/dialogs/NewMessageDialog.qml \
    qml/pages/AddEditEventPage.qml \
    qml/pages/CommentsPage.qml \
    qml/pages/CommentsThreadPage.qml \
    qml/pages/EventPage.qml \
    qml/pages/FriendGroupsPage.qml \
    qml/pages/FriendsListPage.qml \
    qml/pages/FriendsPage.qml \
    qml/pages/FriendsSorterSelectorPage.qml \
    qml/pages/MessagePage.qml \
    qml/pages/MessagesFilterSelectorPage.qml \
    qml/pages/MessagesPage.qml \
    qml/pages/NotificationPage.qml \
    qml/pages/NotificationsPage.qml \
    qml/pages/ProfilePage.qml \
    qml/pages/SplashScreenPage.qml \
    qml/pages/UserJournalPage.qml \
    qml/utils/Utils.js \
    qml/utils/FriendsSorter.qml \
    qml/utils/MessageDirectionFilter.qml \
    qml/harbour-mnemosy.qml \
    rpm/harbour-mnemosy.changes \
    harbour-mnemosy.desktop \
    rpm/harbour-mnemosy.spec \
    qml/pages/EventSettingsPage.qml

lupdate_only{
SOURCES =     qml/components/UserHeaderItem.qml \
    qml/components/EntryHeaderItem.qml \
    qml/components/IconTextButton.qml \
    qml/components/ClickableLabel.qml \
    qml/components/Popup.qml \
    qml/cover/CoverPage.qml \
    qml/dialogs/AddEditCommentDialog.qml \
    qml/dialogs/AddEditFriendDialog.qml \
    qml/dialogs/AddFriendGroupDialog.qml \
    qml/dialogs/AuthorizationDialog.qml \
    qml/dialogs/DeleteCommentDialog.qml \
    qml/dialogs/FilterFriendsPageDialog.qml \
    qml/dialogs/FriendsGroupsSelectorDialog.qml \
    qml/dialogs/SearchUserBlogDialog.qml \
    qml/dialogs/NewMessageDialog.qml \
    qml/pages/AddEditEventPage.qml \
    qml/pages/CommentsPage.qml \
    qml/pages/CommentsThreadPage.qml \
    qml/pages/EventPage.qml \
    qml/pages/FriendGroupsPage.qml \
    qml/pages/FriendsListPage.qml \
    qml/pages/FriendsPage.qml \
    qml/pages/FriendsSorterSelectorPage.qml \
    qml/pages/MessagePage.qml \
    qml/pages/MessagesFilterSelectorPage.qml \
    qml/pages/MessagesPage.qml \
    qml/pages/NotificationPage.qml \
    qml/pages/NotificationsPage.qml \
    qml/pages/ProfilePage.qml \
    qml/pages/SplashScreenPage.qml \
    qml/pages/UserJournalPage.qml \
    qml/utils/Utils.js \
    qml/utils/FriendsSorter.qml \
    qml/utils/MessageDirectionFilter.qml \
    qml/harbour-mnemosy.qml
}

#CONFIG += sailfishapp_i18n
#TRANSLATIONS += translations/harbour-mnemosy-ru.ts
