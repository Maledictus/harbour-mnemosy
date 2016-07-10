TARGET = harbour-mnemosy

VERSION = 0.1.0

CONFIG += link_pkgconfig sailfishapp c++11
PKGCONFIG += dconf

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

RESOURCES += $${TARGET}.qrc

SOURCES += src/main.cpp \
    src/debugmessagehandler.cpp \
    src/application.cpp \
    src/settings/accountsettings.cpp \
    src/dconf/mdconfagent.cpp \
    src/dconf/mdconf.cpp

HEADERS += src/debugmessagehandler.h \
    src/application.h \
    src/settings/accountsettings.h \
    src/dconf/mdconfagent.h \
    src/dconf/mdconf_p.h

OTHER_FILES += harbour-mnemosy.desktop \
        harbour-mnemosy.png \
        rpm/harbour-mnemosy.changes.in \
        rpm/harbour-mnemosy.spec\
        qml/harbour-mnemosy.qml

DISTFILES += \
    qml/pages/AuthorizationDialog.qml
