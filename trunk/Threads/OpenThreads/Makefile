
# Include the header to set up the environment.
include ${CADKIT_MAKE_DIR}/header.mak

# C++ source files.
CPP_FILES = \
	./Mutex.cpp \
	./Thread.cpp

# The include path.
LOCAL_INCLUDE = \
	-I../ \
	-I$(OSG_INC_DIR) \
	-I$(CADKIT_INC_DIR)

# The name of the thing we are making.
TARGET = OpenThreadsEx

include ${CADKIT_MAKE_DIR}/library.mak
