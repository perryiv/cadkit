SOURCE = \
	Parser.cpp \
	Settings.cpp

OS := $(shell uname)
OS_COMP_FLAGS =
OS_LINK_FLAGS =
COMPILER = c++

# Platform dependent flags. Do not indent.
ifeq ($(findstring IRIX,$(OS)),IRIX)
COMPILER = CC
OS_COMP_FLAGS = -LANG:std -I$(BOOST_ROOT_DIR)/boost/compatibility/cpp_c_headers -woff 1183
OS_LINK_FLAGS = -LANG:std -v
endif

COMMON_FLAGS   = -g -D_DEBUG -D_COMPILING_VRV_PREFS
COMPILER_FLAGS = $(COMMON_FLAGS) $(OS_COMP_FLAGS)
LINKER_FLAGS   = $(COMMON_FLAGS) $(OS_LINK_FLAGS)

COMPILE_COMMAND = $(COMPILER) $(COMPILER_FLAGS)
LINK_COMMAND		= $(COMPILER) $(LINKER_FLAGS)

INCLUDES = -I../../ -I$(VJ_BASE_DIR)/include -I$(VJ_DEPS_DIR)/include
LIBS =

TARGET      = vrv_prefs
ARCHIVE_LIB = lib$(TARGET)_a.a
SHARED_LIB  = lib$(TARGET)_s.so
BIN_DIR     = ../../bin

OBJECTS = $(SOURCE:.cpp=.o)

default : $(TARGET)

$(TARGET) : ${OBJECTS}
	rm -rf $(ARCHIVE_LIB)
	ar -cr $(ARCHIVE_LIB) $(OBJECTS)
	$(LINK_COMMAND) -shared -fPIC $(OBJECTS) -o $(SHARED_LIB) $(LIBS)
	mkdir -p $(BIN_DIR)
	mv $(ARCHIVE_LIB) $(BIN_DIR)
	mv $(SHARED_LIB) $(BIN_DIR)

.cpp.o :
	$(COMPILE_COMMAND) $(INCLUDES) -c -fPIC $< -o $@

clean:
	rm -f ${OBJECTS}
	rm -rf ii_files/

clobber:
	rm -f ${OBJECTS} $(ARCHIVE_LIB) $(SHARED_LIB)
	rm -rf ii_files/
