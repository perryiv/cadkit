SOURCE = \
	Adaptors/ProgressBarUpdate.cpp \
	Base/Referenced.cpp \
	Base/Typed.cpp \
	Components/Object.cpp \
	Components/Registry.cpp \
	Components/Manager.cpp \
	DLL/Library.cpp \
	DLL/Loader.cpp \
	Errors/Stack.cpp \
	File/Temp.cpp \
	ID/GUID.cpp \
	Resources/CancelButton.cpp \
	Resources/EventQueue.cpp \
	Resources/ProgressBar.cpp \
	Resources/ReportErrors.cpp \
	Resources/StatusBar.cpp \
	State/Busy.cpp \
	System/LastError.cpp \
	System/Host.cpp \
	System/Screen.cpp \
	System/Socket.cpp \
	Types/Types.cpp \
	Threads/Mutex.cpp

OS := $(shell uname)
OS_COMP_FLAGS =
OS_LINK_FLAGS =
COMPILER = c++

COMMON_FLAGS   = -g -D_DEBUG -D_COMPILING_USUL

# Platform dependent flags. Do not indent.
ifeq ($(findstring IRIX,$(OS)),IRIX)
COMPILER = g++  #CC
OS_COMP_FLAGS = -mabi=n32 -mips4 #-n32 -LANG:std
OS_LINK_FLAGS = -L/usr/freeware/lib32 #-n32 -LANG:std -v
COMMON_FLAGS  = -g -D_DEBUG -D_COMPILING_USUL -D__SGI
endif


COMPILER_FLAGS = $(COMMON_FLAGS) $(OS_COMP_FLAGS)
LINKER_FLAGS   = $(COMMON_FLAGS) $(OS_LINK_FLAGS)

COMPILE_COMMAND = $(COMPILER) $(COMPILER_FLAGS)
LINK_COMMAND		= $(COMPILER) $(LINKER_FLAGS)

INCLUDES = \
	-I../ 


LIBS = 

TARGET      = Usul
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
	$(COMPILE_COMMAND)$(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -rf ii_files/
	cd Adaptors ; $(CLEAN)
	cd Base ; $(CLEAN)
	cd Components ; $(CLEAN)
	cd DLL ; $(CLEAN)
	cd File ; $(CLEAN)
	cd ID ; $(CLEAN)
	cd System ; $(CLEAN)
	cd Types ; $(CLEAN)
	cd Threads ; $(CLEAN)

clobber:
	rm -f $(OBJECTS) $(ARCHIVE_LIB) $(SHARED_LIB)
	rm -rf ii_files/
	cd Adaptors ; $(CLEAN)
	cd Base ; $(CLEAN)
	cd Components ; $(CLEAN)
	cd DLL ; $(CLEAN)
	cd File ; $(CLEAN)
	cd ID ; $(CLEAN)
	cd System ; $(CLEAN)
	cd Types ; $(CLEAN)
	cd Threads ; $(CLEAN)
