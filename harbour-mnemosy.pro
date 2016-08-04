TARGET = harbour-mnemosy

VERSION = 0.2.0

QT += xml xmlpatterns qml

CONFIG += link_pkgconfig sailfishapp c++11
PKGCONFIG += dconf

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

RESOURCES += $${TARGET}.qrc

SOURCES += src/main.cpp \
    src/debugmessagehandler.cpp \
    src/application.cpp \
    src/settings/accountsettings.cpp \
    src/dconf/mdconfagent.cpp \
    src/dconf/mdconf.cpp \
    src/mnemosymanager.cpp \
    src/settings/applicationsettings.cpp \
    src/lj-rpc/ljxmlrpc.cpp \
    src/lj-rpc/rpcutils.cpp \
    src/lj-rpc/ljparsertype.cpp \
    src/userprofile.cpp \
    src/friendsgroup.cpp \
    src/enumsproxy.cpp \
    src/ljevent.cpp \
    src/ljeventproperties.cpp \
    src/models/ljeventsmodel.cpp

HEADERS += src/debugmessagehandler.h \
    src/application.h \
    src/settings/accountsettings.h \
    src/dconf/mdconfagent.h \
    src/dconf/mdconf_p.h \
    src/mnemosymanager.h \
    src/settings/applicationsettings.h \
    src/lj-rpc/ljxmlrpc.h \
    src/lj-rpc/rpcutils.h \
    src/lj-rpc/ljparsertype.h \
    src/userprofile.h \
    src/friendsgroup.h \
    src/enumsproxy.h \
    src/ljevent.h \
    src/ljeventproperties.h \
    src/models/ljeventsmodel.h

OTHER_FILES += harbour-mnemosy.desktop \
        harbour-mnemosy.png \
        rpm/harbour-mnemosy.changes.in \
        rpm/harbour-mnemosy.spec\
        qml/harbour-mnemosy.qml

DISTFILES += \
    qml/pages/AuthorizationDialog.qml \
    qml/pages/ProfilePage.qml \
    qml/components/UserHeaderItem.qml \
    qml/pages/FriendsPage.qml \
    qml/components/EntryHeaderItem.qml \
    qml/utils/Utils.js \
    qml/pages/EventPage.qml \
    qml/components/IconTextButton.qml
