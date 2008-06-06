TEMPLATE = lib \
    dll
DESTDIR = ${CADKIT_BIN_DIR}
CONFIG += debug_and_release
HEADERS += ./Application.h \
    ./Constants.h \
    ./DocumentProxy.h \
    ./MainWindow.h \
    ./ProgressBarDock.h \
    ./SplashScreen.h
INCLUDEPATH += ${CADKIT_INC_DIR} \
    ${CADKIT_INC_DIR}/Experimental \
    ${QTDIR}/include
LIBS += -L${CADKIT_BIN_DIR}
CONFIG(debug, debug|release) { 
    LIBS += -lUsuld \
        -lQtToolsd \
        -lMenuKitd \
        -lHeliosQtCommandsd \
        -lHeliosQtToolsd
    TARGET = HeliosQtCored
}
else { 
    TARGET = HeliosQtCore
    LIBS += -lUsul \
        -lQtTools
}
QMAKE_CXXFLAGS += -fPIC
QMAKE_LFLAGS += -fPIC
SOURCES += ./Application.cpp \
    ./DocumentProxy.cpp \
    ./MainWindow.cpp \
    ./ProgressBarDock.cpp \
    ./SplashScreen.cpp
QT += core \
    gui
