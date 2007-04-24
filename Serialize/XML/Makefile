# Include the header to set up the environment.
include ${USUL_INC_DIR}/Usul/Make/header.mak

# C++ source files.
CPP_FILES = \
	./Factory.cpp \
	./DataMemberMap.cpp \
	./BaseCreator.cpp \
	./MemberBase.cpp \

# The include path.
LOCAL_INCLUDE = \
	-I$(CADKIT_ROOT_DIR)\
	-I$(CADKIT_ROOT_DIR)/Experimental \
	-I$(XERCES_INC_DIR)

# The libraries to link to.
LOCAL_LIBS = \
	-L$(XERCES_LIB_DIR) -lxerces-c

# Any flags for the compiler. 
LOCAL_CPP_FLAGS = 

# The name of the thing we are making.
TARGET = XmlSerialize

# The common rules for compiling are in here.
include ${USUL_INC_DIR}/Usul/Make/library.mak
