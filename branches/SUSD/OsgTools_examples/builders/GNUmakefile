SHELL = /bin/sh
LIB_OS = lib
OS := $(shell uname)

default : exe

# Platform dependent flags. Do not indent.
ifeq ($(findstring IRIX,$(OS)),IRIX)
OS := IRIX32
CXX = CC -n32 -LANG:std -woff 1682 -mips3
CXXFLAGS =
LIB_OS = lib32
endif

libs = \
       -L$(OSG_LIB_DIR) -losg -losgProducer -losgUtil -losgDB -losgGA -losgText \
       -L$(OPENTHREADS_LIB_DIR) -lOpenThreads \
       -L$(PRODUCER_LIB_DIR) -lProducer \
       -L$(OSGTOOLS_LIB_DIR) -lOsgTools_s \
       -L$(USUL_LIB_DIR) -lUsul_s \
       -L/usr/$(LIB_OS) -lm

includes = \
           -I$(OSG_INC_DIR) \
           -I$(OPENTHREADS_INC_DIR) \
           -I$(PRODUCER_INC_DIR) \
           -I$(OSGTOOLS_INC_DIR)

flags = -g

BUILD=$(OS)
exec_name = builders.exe

OBJS= \
	$(BUILD)/main.o

exe : $(OBJS)
	$(CXX) $(OBJS) -o $(BUILD)/$(exec_name) $(libs) -v


$(OBJS): $(BUILD)/%.o: %.cpp
	mkdir -p $(BUILD)
	@echo "Compiling $< into $@"
	$(CXX) $(flags) $(CXXFLAGS) $(includes) -c $< -o $@

clean:
	$(RM) -r $(BUILD) so_locations
