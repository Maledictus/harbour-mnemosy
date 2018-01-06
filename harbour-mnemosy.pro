TARGET = harbour-mnemosy

VERSION = 0.7

QT += xml xmlpatterns qml

CONFIG += link_pkgconfig sailfishapp c++11 sailfishapp_i18n
PKGCONFIG += mlite5

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

RESOURCES += $${TARGET}.qrc

SOURCES += src/main.cpp \
    src/lj-rpc/ljxmlrpc.cpp \
    src/lj-rpc/rpcutils.cpp \
    src/lj-rpc/ljparsertype.cpp \
    src/models/ljeventsmodel.cpp \
    src/models/ljcommentsmodel.cpp \
    src/models/ljmessagesmodel.cpp \
    src/models/ljfriendsmodel.cpp \
    src/models/messagessortfilterproxymodel.cpp \
    src/models/ljfriendsgroupsmodel.cpp \
    src/models/friendssortfilterproxymodel.cpp \
    src/settings/accountsettings.cpp \
    src/settings/applicationsettings.cpp \
    src/application.cpp \
    src/debugmessagehandler.cpp \
    src/enumsproxy.cpp \
    src/mnemosymanager.cpp \
    src/ljcomment.cpp \
    src/ljcommentproperties.cpp \
    src/ljevent.cpp \
    src/ljeventproperties.cpp \
    src/ljfriend.cpp \
    src/ljfriendsgroup.cpp \
    src/ljmessage.cpp \
    src/userprofile.cpp \
    src/utils.cpp

HEADERS += src/lj-rpc/ljxmlrpc.h \
    src/lj-rpc/rpcutils.h \
    src/lj-rpc/ljparsertype.h \
    src/models/ljmessagesmodel.h \
    src/models/cachedmodel.h \
    src/models/ljfriendsmodel.h \
    src/models/messagessortfilterproxymodel.h \
    src/models/friendssortfilterproxymodel.h \
    src/models/ljfriendsgroupsmodel.h \
    src/models/ljcommentsmodel.h \
    src/models/ljeventsmodel.h \
    src/settings/accountsettings.h \
    src/settings/applicationsettings.h \
    src/application.h \
    src/debugmessagehandler.h \
    src/enumsproxy.h \
    src/ljcomment.h \
    src/ljcommentproperties.h \
    src/ljevent.h \
    src/ljeventproperties.h \
    src/ljfriend.h \
    src/ljfriendsgroup.h \
    src/ljmessage.h \
    src/mnemosymanager.h \
    src/userprofile.h \
    src/utils.h

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

DISTFILES += \
    qml/components/UserHeaderItem.qml \
    qml/components/EntryHeaderItem.qml \
    qml/components/IconTextButton.qml \
    qml/components/ClickableLabel.qml \
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
    rpm/harbour-mnemosy.yaml \
    qml/pages/EventSettingsPage.qml

lupdate_only{
SOURCES = qml/components/UserHeaderItem.qml \
    qml/components/EntryHeaderItem.qml \
    qml/components/IconTextButton.qml \
    qml/components/ClickableLabel.qml \
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
    qml/pages/EventSettingsPage.qml
}

TRANSLATIONS += translations/harbour-mnemosy.ts \
    translations/harbour-mnemosy-ru.ts \
    translations/harbour-mnemosy-en.ts
