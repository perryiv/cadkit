
default:	all

APP_NAME=	Minerva


EXTRA_INCLUDES = \
	-I./ \
	-I../ \
	-I../../../../ \
	-I../../include \
	-I${PQXX_INC_DIR} \
	-I${VJ_BASE_DIR}/include \
	-I${VJ_DEPS_DIR}/include \
	-I$(VJ_BASE_DIR)/include/tweek/CORBA \
	-I$(VJ_BASE_DIR)/include/tweek/idl/ \
	-I${OSG_INC_DIR} \
	-I${OPENTHREADS_INC_DIR} \
	-I${VRJCINEMA_ROOT_DIR}/src \
	-I$(OSSIM_INC_DIR) \
	-I$(OSSIM_PLANET_INC_DIR) \
	-I$(OSG_EPHEMERIS_INC_DIR) \
	-I$(CADKIT_ROOT_DIR) \
	-I$(CADKIT_ROOT_DIR)/Experimental \
	-I$(PRODUCER_INC_DIR) 

LOCAL_LIBS = \
	-L$(CADKIT_ROOT_DIR)/bin \
	-lOsgTools \
	-lUsul \
	-lOpenThreadsEx \
	-lMinerva \
	-lMagrathea \
	-lXmlSerialize \
	-lXmlTree \
	-L${VRJCINEMA_ROOT_DIR}/lib \
	-lVrjCinema \
	-L$(PQXX_LIB_DIR) \
	-lpqxx \
	-L$(CGAL_LIB_DIR) \
	-lCGAL \
	-L$(BOOST_LIB_DIR) \
	-L$(OSSIM_LIB_DIR) \
	-lossim \
	-L$(OSSIM_PLANET_LIB_DIR) \
	-lossimPlanet \
	-L$(OSG_EPHEMERIS_LIB_DIR) \
	-losgEphemeris \
	-L$(XERCES_LIB_DIR) \
	-lxerces-c


JUGGLER_LIBS = \
	-L${JUGGLER_LIB_DIR} \
	-lvrj \
	-lvrj_ogl \
	-lsonix \
	-lvpr \
	-lgadget \
	-ljccl \
	-L${VJ_DEPS_DIR}/lib \
	-lcppdom 

OSG_LIBS = \
	-L${OSG_LIB_DIR} \
	-losgText \
	-losgUtil \
	-losgFX \
	-losgDB \
	-losg 

OPEN_THREADS_LIBS = \
	-L${OPENTHREADS_LIB_DIR} \
	-lOpenThreads

STANDARD_LIBS = \
	-lGL \
	-lm


# Basic options.
#-------------------------------------------------------

#BUILD_TYPE=	opt
BUILD_TYPE=	dbg

srcdir=		./

SRCS=		main.cpp		\
		MinervaVR.cpp

NO_DEPEND=	YES

EXTRA_LIBS=$(LOCAL_LIBS) $(JUGGLER_LIBS) $(OSG_LIBS) $(OPEN_THREADS_LIBS) $(STANDARD_LIBS)

# Advanced options. 
#---------------------------------------------------------------

include $(VJ_BASE_DIR)/share/vrjuggler/vrj.appdefs.mk
include $(VJ_BASE_DIR)/share/Doozer/ext/vrjuggler/dzr.vrjuggler.glapp.mk
include $(VJ_BASE_DIR)/share/Doozer/ext/tweek/dzr.tweek.mk



#vpath %.cpp $(CXX_GEN_DIR)
#vpath %.h $(CXX_GEN_DIR)

# -----------------------------------------------------------------------------
# Application build targets.
# -----------------------------------------------------------------------------
all: 		$(APP_NAME)

$(APP_NAME): 	$(OBJS)
		$(LINK) $(LINK_OUT)$@ $(OBJS) $(TWEEK_LIBS) $(EXTRA_LIBS) $(LIBS)
