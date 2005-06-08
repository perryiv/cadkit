SOURCE = \
	Adaptors/Wait.cpp \
	Cursors/Factory.cpp \
	Dialogs/FileSelection.cpp \
	Dialogs/Message.cpp \
	Functions/App.cpp \
	Functions/Check.cpp \
	Functions/Delete.cpp \
	Functions/Enable.cpp \
	Functions/Files.cpp \
	Functions/GLVisual.cpp \
	Functions/MainWindow.cpp \
	Functions/Mouse.cpp \
	Icons/Factory.cpp \
	Menu/Bar.cpp \
	Menu/Button.cpp \
	Menu/CheckButton.cpp \
	Menu/Command.cpp \
	Menu/Group.cpp \
	Menu/Item.cpp \
	Menu/RadioButton.cpp \
	Menu/RecentFiles.cpp \
	Menu/Separator.cpp \
	ToolBar/Button.cpp \
	Widgets/SliderTextField.cpp

OS := $(shell uname)
OS_COMP_FLAGS =
OS_LINK_FLAGS =
COMPILER = c++



# Platform dependent flags. Do not indent.
ifeq ($(findstring IRIX,$(OS)),IRIX)
COMPILER = g++ #CC
OS_COMP_FLAGS = -mabi=n32 -mips4 #-n32 -LANG:std
OS_LINK_FLAGS = -L/usr/freeware/lib32 #-n32 -LANG:std -v
endif

COMMON_FLAGS   = -g -D_DEBUG -D_COMPILING_FOX_TOOLS
COMPILER_FLAGS = $(COMMON_FLAGS) $(OS_COMP_FLAGS)
LINKER_FLAGS   = $(COMMON_FLAGS) $(OS_LINK_FLAGS)

COMPILE_COMMAND = $(COMPILER) $(COMPILER_FLAGS)
LINK_COMMAND		= $(COMPILER) $(LINKER_FLAGS)

INCLUDES = \
	-I../ \
	-I${FOX_INC_DIR}


LIBS = \
	-L${FOX_LIB_DIR} -lFOX-1.4

TARGET      = FoxTools
ARCHIVE_LIB = lib$(TARGET)_a.a
SHARED_LIB  = lib$(TARGET)_s.so
BIN_DIR     = ../bin
CLEAN       = rm -rf ii_files/ ; rm -f *.o ; cd ../

OBJECTS = $(SOURCE:.cpp=.o)

default : $(TARGET)

$(TARGET) : $(OBJECTS)
	rm -rf $(ARCHIVE_LIB)
	ar -cr $(ARCHIVE_LIB) $(OBJECTS)
	$(LINK_COMMAND) -shared $(OBJECTS) -o $(SHARED_LIB) $(LIBS)
	mkdir -p $(BIN_DIR)
	mv $(ARCHIVE_LIB) $(BIN_DIR)
	mv $(SHARED_LIB) $(BIN_DIR)

.cpp.o :
	$(COMPILE_COMMAND) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -rf ii_files/
	cd Icons ; $(CLEAN)
	cd Cursors ; $(CLEAN)
	cd Functions ; $(CLEAN)
	cd Dialogs ; $(CLEAN)
	cd Menu ; $(CLEAN)

clobber:
	rm -f $(OBJECTS) $(ARCHIVE_LIB) $(SHARED_LIB)
	rm -rf ii_files/
	cd Icons ; $(CLEAN)
	cd Cursors ; $(CLEAN)
	cd Functions ; $(CLEAN)
	cd Dialogs ; $(CLEAN)
	cd Menu ; $(CLEAN)