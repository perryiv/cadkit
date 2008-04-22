LIB_SOURCE = \
	Tracker.cpp \
	Station.cpp \
	Referenced.cpp

APP_SOURCE = \
	$(LIB_SOURCE) \
	main.cpp

COMPILER = c++
#COMPILER = CC

FLAGS = -g -D_DEBUG -D_COMPILING_INTERSENSE_LIBRARY

COMPILE_COMMAND = $(COMPILER) $(FLAGS)
LINK_COMMAND		= $(COMPILER) $(FLAGS)

INCLUDES = -I../ -I$(INTERSENSE_SDK) -I$(VJ_BASE_DIR)/include
LIBS = -L${INTERSENSE_SDK}/isense/Linux -lisense

APP_TARGET = test_tracker
LIB_TARGET = InterSense

APP_OBJECTS = $(APP_SOURCE:.cpp=.o)
LIB_OBJECTS = $(LIB_SOURCE:.cpp=.o)

default : $(LIB_TARGET)

$(APP_TARGET) : ${APP_OBJECTS}
	${LINK_COMMAND} ${APP_OBJECTS} -o $(APP_TARGET) ${LIBS}

$(LIB_TARGET) : ${LIB_OBJECTS}
	rm -rf lib$(LIB_TARGET)_a.a
	ar -cr lib$(LIB_TARGET)_a.a $(LIB_OBJECTS)
	$(LINK_COMMAND) -shared $(LIB_OBJECTS) -o lib$(LIB_TARGET)_s.so $(LIBS)

.cpp.o :
	$(COMPILE_COMMAND) $(INCLUDES) -c $< -o $@

clean:
	rm -f ${APP_OBJECTS}
	rm -rf ii_files/

clobber:
	rm -f ${APP_OBJECTS} ${APP_TARGET} lib${LIB_TARGET}_a.a lib${LIB_TARGET}_s.so
	rm -rf ii_files/
