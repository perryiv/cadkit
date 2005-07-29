SOURCE = \
	Application.cpp \
	AppCallbacks.cpp \
	main.cpp \
	Registry.cpp \
	ConfigFiles.cpp \
	Functors/AnalogInput.cpp \
	Functors/BaseFunctor.cpp \
	Functors/Direction.cpp \
	Functors/IdentityMatrix.cpp \
	Functors/InverseMatrix.cpp \
	Functors/JoystickHorizontal.cpp \
	Functors/JoystickVertical.cpp \
	Functors/MatrixFunctor.cpp \
	Functors/MatrixPair.cpp \
	Functors/Rotate.cpp \
	Functors/Transform.cpp \
	Functors/TransformPair.cpp \
	Functors/Translate.cpp \
	Functors/WandMatrix.cpp \
	Functors/WandPosition.cpp \
	Functors/WandRotation.cpp \
	Functors/Tool.cpp \
	Functors/ToolPair.cpp \
	Functors/ScaleTool.cpp \
	Functors/MoveTool.cpp \
	Pick/Intersect.cpp \
	Pick/Seek.cpp \
	Pick/Select.cpp

OS := $(shell uname)
COMPILER        = c++
DEBUG_FLAGS     = -g -D_DEBUG -D_DEBUG_PRINT
#COMPILER_FLAGS  = -I/usr/include/glib-1.2 -I/usr/lib/glib/include/ -DHAVE_GLIB
#LINKER_FLAGS    = -lglib
JUGGLER_LIB_DIR = ${VJ_BASE_DIR}/lib

# Change some settings when on irix.
ifeq ($(findstring IRIX,$(OS)),IRIX)
COMPILER        = CC
COMPILER_FLAGS  = -LANG:std -I$(BOOST_ROOT_DIR)/boost/compatibility/cpp_c_headers -woff 1681,1682,1183
LINKER_FLAGS    = -LANG:std -v
JUGGLER_LIB_DIR = ${VJ_BASE_DIR}/lib32
endif

# Change some settings when on Cygwin.
ifeq ($(findstring Cygwin,$(OS)),Cygwin)
COMPILER_FLAGS = -I$(NSPR_ROOT)/include
COMPILER = g++
endif

COMPILE_COMMAND = $(COMPILER) $(DEBUG_FLAGS) $(COMPILER_FLAGS)
LINK_COMMAND		= $(COMPILER) $(DEBUG_FLAGS) $(LINKER_FLAGS)

BIN_DIR = ../bin

INCLUDES = \
	-I./ \
	-I../ \
	-I${VJ_BASE_DIR}/include \
	-I${OSG_INC_DIR}

LOCAL_LIBS = \
	-L$(BIN_DIR) \
	-lOsgTools_s \
	-lUsul_s \
	-losgVRJ \
	-lvrjGA \
	-lMenuKit \
	-lMenuKit_OSG \
	-lvrv_prefs_s

JUGGLER_LIBS = \
	-L${JUGGLER_LIB_DIR} \
	-lvrj \
	-lvrj_ogl \
	-lsonix \
	-lvpr \
	-lgadget \
	-ljccl

OSG_LIBS = \
	-L${OSG_LIB_DIR} \
	-losgText \
	-losgUtil \
	-losgFX \
	-losgDB -losg

OPEN_THREADS_LIBS = \
	-L${OPENTHREADS_LIB_DIR} \
	-lOpenThreads

STANDARD_LIBS = \
	-lGL \
	-lm

LIBS = $(LOCAL_LIBS) $(JUGGLER_LIBS) $(OSG_LIBS) $(OPEN_THREADS_LIBS) $(STANDARD_LIBS)

TARGET  = cv
OBJECTS = $(SOURCE:.cpp=.o)
CLEAN   = rm -rf ii_files/ ; rm -f *.o ; cd ../

default : $(TARGET)

$(TARGET) : ${OBJECTS}
	${LINK_COMMAND} ${OBJECTS} -o $(TARGET) ${LIBS}
	mkdir -p $(BIN_DIR)
	mv $(TARGET) $(BIN_DIR)

.cpp.o :
	$(COMPILE_COMMAND) $(INCLUDES) -c $< -o $@

clean:
	rm -f ${TARGET} ${OBJECTS} *.o
	rm -rf ii_files/
	cd Functors ; $(CLEAN)
	cd Pick ; $(CLEAN)

clobber:
	rm -f ${TARGET} ${OBJECTS} *.o
	rm -rf ii_files/
	cd Functors ; $(CLEAN)
	cd Pick ; $(CLEAN)
