# Include the header to set up the environment.
include $(CADKIT_ROOT_DIR)/Make/header.mak

# C++ source files.
CPP_FILES = \
	DbPfArraySetters.cpp \
	DbPfDatabase.cpp \
	DbPfDll.cpp \
	DbPfFunctions.cpp \
	DbPfPrecompiled.cpp

# The include path.
LOCAL_INCLUDE = -I$(OSGHOME)/include

# The libraries to link to.
LOCAL_LIBS = \
	-lCkSl_s \
	-lCkDbBase_s \
	-lpfutil \
	-lpf

# Any flags for the compiler. 
OCAL_CPP_FLAGS =

# The name of the thing we are making.
TARGET = CkDbPf

# The common rules for compiling are in here.
include $(CADKIT_ROOT_DIR)/Make/library.mak
