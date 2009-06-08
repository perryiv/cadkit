TEMPLATE = lib \
    dll
CONFIG += debug_and_release
SOURCES += ./Action.cpp \
    ./Menu.cpp \
    ./Image.cpp \
    ./TreeControl.cpp \
    ./AnimationControl.cpp \
    ./PropertyBrowser.cpp \
    ./ColorButton.cpp
HEADERS += ./AnimationControl.h \
    ./ComboBox.h \
    ./Action.h \
    ./ColorButton.h \
    ./PropertyBrowser.h \
    ./Export.h \
    ./Image.h \
    ./Color.h \
    ./TreeControl.h \
    ./Menu.h
QT += core \
    gui
INCLUDEPATH += ${CADKIT_INC_DIR} \
    ${CADKIT_INC_DIR}/Experimental \
    ${QTDIR}/include
LIBS += -L${CADKIT_BIN_DIR}
CONFIG(debug, debug|release) { 
    LIBS += -lUsuld \
        -lMenuKitd
    TARGET = QtToolsd
}
else { 
    TARGET = QtTools
    LIBS += -lUsul
}
QMAKE_CXXFLAGS += -fPIC
QMAKE_LFLAGS += -fPIC
FORMS += AnimationControl.ui
DESTDIR += ${CADKIT_LIB_DIR}
