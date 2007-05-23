default : shared_lib
PKG_NAME = MenuKit

SHELL = /bin/sh
OS := $(shell uname)
CLEAN = $(RM) -r $(BUILD)

ifeq ($(findstring IRIX,$(OS)),IRIX)
OS := IRIX32
CXX = CC -n32 -LANG:std -woff 1682 -mips3
PLATFORM_INCLUDES = -I$(VJ_BASE_DIR)/include/boost/compatibility/cpp_c_headers
LIB_OS = lib32
CLEAN = $(RM) -r $(BUILD) so_locations
endif

includes = -I./ -I../ -I$(BOOST_ROOT_DIR) $(PLATFORM_INCLUDES)

BUILD=$(OS)
EXT = so
name = $(PKG_NAME)
libname = lib$(name).$(EXT)

OBJS = \
	$(BUILD)/Behavior.o \
	$(BUILD)/Button.o \
	$(BUILD)/Callback.o \
	$(BUILD)/CloseVisitor.o \
	$(BUILD)/Errors.o \
	$(BUILD)/FlagsVisitor.o \
	$(BUILD)/Item.o \
	$(BUILD)/Menu.o \
	$(BUILD)/Referenced.o \
	$(BUILD)/StreamVisitor.o \
	$(BUILD)/Visitor.o \
	$(BUILD)/MessageVisitor.o

shared_lib: $(OBJS)
	$(CXX) -shared -g -fPIC $(OBJS) -o $(BUILD)/$(libname)
	mkdir -p ../bin
	cp $(BUILD)/$(libname) ../bin/

$(OBJS): $(BUILD)/%.o: %.cpp
	mkdir -p $(BUILD)
	@echo "Compiling $< into $@"
	$(CXX) -g -fPIC $(includes) -c $< -o $@

clean:
	$(CLEAN)

instlink:
	ln -sf $(PWD)/$(BUILD)/$(libname) $(INST_LOCATION)/$(libname)

instclean:
	$(RM) $(INST_LOCATION)/$(libname)

mvlibs:
	mkdir -p $(MOVE_LOCATION)
	mv ./$(OS)/*.so $(MOVE_LOCATION)
