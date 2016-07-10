TARGET = harbour-mnemosy

VERSION = 0.1.0

CONFIG += link_pkgconfig sailfishapp c++11
PKGCONFIG += dconf

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

RESOURCES += $${TARGET}.qrc

SOURCES += src/main.cpp \
    src/debugmessagehandler.cpp \
    src/application.cpp

HEADERS += src/debugmessagehandler.h \
    src/application.h

OTHER_FILES += harbour-mnemosy.desktop \
        harbour-mnemosy.png \
        rpm/harbour-mnemosy.changes.in \
        rpm/harbour-mnemosy.spec\
        qml/harbour-mnemosy.qml

DISTFILES += \
    qml/pages/AuthorizationDialog.qml
