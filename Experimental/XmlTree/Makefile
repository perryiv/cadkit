# Include the header to set up the environment.
include ${USUL_INC_DIR}/Usul/Make/header.mak

# C++ source files.
CPP_FILES = \
	./Document.cpp \
	./Node.cpp \
	./Loader.cpp \
	./XercesLife.cpp \
	./Writer.cpp \
	./Detail/RootImpl.cpp \
	./Functions.cpp \


# The include path.
LOCAL_INCLUDE = \
	-I$(CADKIT_ROOT_DIR)\
	-I$(CADKIT_ROOT_DIR)/Experimental

# The libraries to link to.
LOCAL_LIBS = \
	-lxerces-c

# Any flags for the compiler. 
LOCAL_CPP_FLAGS = 

# The name of the thing we are making.
TARGET = XmlTree

# The common rules for compiling are in here.
include ${USUL_INC_DIR}/Usul/Make/library.mak
