TARGET = harbour-mnemosy

VERSION = 0.1.0

CONFIG += link_pkgconfig sailfishapp c++11
PKGCONFIG += dconf

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += src/main.cpp \
        src/debugmessagehandler.cpp

HEADERS += src/debugmessagehandler.h

OTHER_FILES += harbour-mnemosy.desktop \
        harbour-mnemosy.png \
        rpm/harbour-mnemosy.changes.in \
        rpm/harbour-mnemosy.spec
