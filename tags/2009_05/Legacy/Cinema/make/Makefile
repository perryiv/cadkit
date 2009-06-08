# Include the header to set up the environment.
include $(USUL_INC_DIR)/Usul/Make/header.mak

# C++ source files.
CPP_FILES = \
	../src/Cinema/Core/Global.cpp			\
	../src/Cinema/Core/Movie.cpp			\
	../src/Cinema/Core/MovieHandler.cpp		\
	

# The include path.
LOCAL_INCLUDE = \
	-I$(OSG_INC_DIR) 				\
	-I$(OPENTHREADS_INC_DIR) 			\
	-I$(OSSIM_INC_DIR) 				\
	-I$(OSSIM_PLANET_INC_DIR) 			\
	-I$(OSG_EPHEMERIS_INC_DIR) 			\
	-I$(BOOST_INC_DIR) 				\
	-I$(LIBWMS_INC_DIR)				\
	-I../src					\
	-I../../../

# The libraries to link to.
LOCAL_LIBS = \
	-L$(USUL_LIB_DIR) -lOsgTools -lUsul 		\
	-L$(OSSIM_LIB_DIR) -lossim 			\
	-L$(OSSIM_PLANET_LIB_DIR) -lossimPlanet 	\
	-L$(BOOST_LIB_DIR)				\
	-L$(LIBWMS_LIB_DIR) -lwms


# Any flags for the compiler. 
LOCAL_CPP_FLAGS = 

# The name of the thing we are making.
TARGET = Cinema

# The common rules for compiling are in here.
include $(USUL_INC_DIR)/Usul/Make/library.mak
