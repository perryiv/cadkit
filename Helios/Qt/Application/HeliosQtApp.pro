TEMPLATE = app
DESTDIR = ${CADKIT_BIN_DIR}
CONFIG += debug_and_release
SOURCES += ./main.cpp
QT += core \
    gui
INCLUDEPATH += ${CADKIT_INC_DIR} \
    ${CADKIT_INC_DIR}/Experimental \
    ${QTDIR}/include
LIBS += -L${CADKIT_BIN_DIR}
CONFIG(debug, debug|release) { 
    LIBS += -lUsuld \
        -lHeliosQtCored \
        -lOpenThreadsExd \
        -lXmlTreed
    TARGET = HeliosQtd
}
else { 
    TARGET = HeliosQt
    LIBS += -lUsul \
        -lQtTools \
        -lHeliosQtCore
}
QMAKE_CXXFLAGS += -fPIC
QMAKE_LFLAGS += -fPIC
