SOURCE = \
	ReaderWriterCRSS.cpp 

OS := $(shell uname)
OS_COMP_FLAGS =
OS_LINK_FLAGS = -shared
COMPILER = c++

# Platform dependent flags. Do not indent.
ifeq ($(findstring Darwin,$(OS)),Darwin)
OS_LINK_FLAGS = -dynamiclib
endif

ifeq ($(findstring IRIX,$(OS)),IRIX)
COMPILER = CC
OS_COMP_FLAGS = -LANG:std
OS_LINK_FLAGS = -LANG:std -v
endif

COMMON_FLAGS   = -g
COMPILER_FLAGS = $(COMMON_FLAGS) $(OS_COMP_FLAGS)
LINKER_FLAGS   = $(COMMON_FLAGS) $(OS_LINK_FLAGS)

COMPILE_COMMAND = $(COMPILER) $(COMPILER_FLAGS)
LINK_COMMAND	= $(COMPILER) $(LINKER_FLAGS)

INCLUDES = \
	-I../../ \
	-I${OSG_INC_DIR} \
	-I${USUL_INC_DIR} \
	-I${OPENTHREADS_INC_DIR} \
	-I${PRODUCER_INC_DIR} \
	-I${OSG_TOOLS_INC_DIR}

LIBDIRS = \
	-L${OSG_LIB_DIR} \
	-L${OPENTHREADS_LIB_DIR} \
	-L${PRODUCER_LIB_DIR} \
	-L${USUL_LIB_DIR} \
	-L${OSG_TOOLS_LIB_DIR}
	

LIBS = \
	-losgDB -losg -losgUtil -losgText -lProducer -lOpenThreads \
	-lUsul -lOsgTools


TARGET      = osgdb_crss
ARCHIVE_LIB = lib$(TARGET)_a.a
SHARED_LIB  = $(TARGET).so

OSGPLUGIN_DIR     = ${OSG_LIB_DIR}/osgPlugins
CLEAN       = rm -rf ii_files/ ; rm -f *.o ; cd ../

OBJECTS = $(SOURCE:.cpp=.o)

all : default 

default : $(TARGET)

$(TARGET) : $(OBJECTS)
	rm -rf $(ARCHIVE_LIB)
	ar -cr $(ARCHIVE_LIB) $(OBJECTS)
	$(LINK_COMMAND) $(OBJECTS) -o $(SHARED_LIB) $(LIBDIRS) $(LIBS)
	mkdir -p $(OSGPLUGIN_DIR)
	
	mv $(SHARED_LIB) $(OSGPLUGIN_DIR)

.cpp.o :
	$(COMPILE_COMMAND) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -rf ii_files/

clobber:
	rm -f $(OBJECTS) $(ARCHIVE_LIB) $(SHARED_LIB)
	rm -rf ii_files/
