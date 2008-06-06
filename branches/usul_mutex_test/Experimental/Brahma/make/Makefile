# Include the header to set up the environment.
include ${USUL_INC_DIR}/Usul/Make/header.mak

# C++ source files.
CPP_FILES = \
	Brahma/Core/Brahma.cpp		\
	Brahma/Scene/Group.cpp		\
	Brahma/Scene/Node.cpp		\
	Brahma/Scene/Transform.cpp	\


# The include path.
LOCAL_INCLUDE = \
	-I$(BOOST_INC_DIR) 		\
	-I$(USUL_INC_DIR)			\
	-I$(LOCAL_SRC_DIR)		 		


# The libraries to link to.
LOCAL_LIBS = \
	-L$(USUL_LIB_DIR) -lUsul 						\
	-L$(BOOST_LIB_DIR)							\
	-L$(LOCAL_SRC_DIR)/bin -lUsul -lXmlTree -lXmlSerialize 	


# Any flags for the compiler. 
LOCAL_CPP_FLAGS = 

# The name of the thing we are making.
TARGET = Brahma

# The common rules for compiling are in here.
include ${USUL_INC_DIR}/Usul/Make/library.mak
