# Include the header to set up the environment.
include ${USUL_INC_DIR}/Usul/Make/header.mak

# C++ source files.
CPP_FILES = \
	./Main.cpp

# The include path.
LOCAL_INCLUDE = \
	-I$(CADKIT_ROOT_DIR)\
	-I$(CADKIT_ROOT_DIR)/Experimental

# The libraries to link to.
LOCAL_LIBS = \
	-L$(CADKIT_ROOT_DIR)/bin -lXmlSerialize_s -lUsul -lXmlTree_s -L/lib64 -ldl \
	-lxerces-c

# Any flags for the compiler. 
LOCAL_CPP_FLAGS = 

# The name of the thing we are making.
TARGET = SerializeTest01

# The common rules for compiling are in here.
include ${USUL_INC_DIR}/Usul/Make/executable.mak
