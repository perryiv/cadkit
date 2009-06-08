#/**********************************************************************
# * @Author: Aashish Chaudhary
# *          Sr. Research Analyst at DT   
# *          Arizona State University 
# *
# * @Date: Jan 23 2006
# *
# *
# * @Comment:
# *	This is a example makefile. This makefile assumes that 
# *  user is using VJCinema framework along with VRJuggler. 
# *  OSG, OGL and OpenSG are optional though this makefile 
# *  using OSG. This makefile is a standard gnu makefile using 
# *  basic tools from VRJuggler to make it portable alond 
# *  different architecture. Its higly recommended that you learn 
# *  gnu make to understand some basic described here. 
# *
# *  This makefile is divided into two parts basic and advanced. 
# *  Which is described in detail below. 
# * 
# * @Note:
# *	Only standard variables defined here. 
# *
# *	Basic options.
# *  -------------------------------------------------------------------
# *	1)	srcdir
# *		srcdir=		../src
# *
# *	This variable should be used to define the primary dir for the src 
# *	files. If you want to add more src dir then use EXTRA_PATH_FOR_SOURCES. 
# *
# *	2)	EXTRA_PATH_FOR_SOURCES
# *		
# *		EXTRA_PATH_FOR_SOURCES=		path1	\
# *						path2	
# * 
# *  This variable defines where the makefile will look for the src
# *	files. If there are more than one directory then they should be 
# *  included by a backslash. 
# *	
# **********************************************************************
# */

default:	all

APP_NAME=	VJViewer


# Basic options.
#-------------------------------------------------------
BUILD_TYPE?=	opt

EXTRA_CXXFLAGS= -o2 -DVTK5

srcdir=				./

EXTRA_PATH_FOR_SOURCES=		../DataObject	\
				../App		\
				../Layer	\
				../Parser	\
				../Scene	\
				../SimFlowUtil	\
				../VTKUtil		

SRCS=		ISAX2Handler.cpp	\
		AppSAX2Handler.cpp	\
		Parser.cpp		\
		DataObject.cpp		\
		Point.cpp		\
		Polygon.cpp		\
		Layer.cpp		\
		PointLayer.cpp		\
		PointTimeLayer.cpp	\
		PolygonLayer.cpp	\
		SceneManager.cpp	\
		VTKToOSG.cpp		\
		MyApp.cpp		\
		main.cpp

NO_DEPEND=	YES
		
EXTRA_INCLUDES=		-I../ -I$(VJCINEMA_ROOT_DIR)/include -I$(OSG_INC_DIR) -I$(OPENTHREADS_INC_DIR) -I$(PRODUCER_INC_DIR) \
			-I$(VJ_BASE_DIR)/include/tweek/idl -I$(VJ_BASE_DIR)/include -I$(VJ_BASE_DIR)/include/tweek/CORBA	\
			-I$(VTK_INC_DIR) -I$(XERCES_INC_DIR) -I$(VTK_INC_DIR) -I$(VTK_INC_DIR)/Graphics -I$(VTK_INC_DIR)/Common	\
			-I$(VTK_INC_DIR)/Imaging -I$(VTK_INC_DIR)/Filtering -I$(VTK_INC_DIR)/Rendering -I$(VTK_INC_DIR)/build.rh4.64	\
			-I$(VTK_INC_DIR)/IO 

EXTRA_LIBS=		-L$(VJCINEMA_ROOT_DIR)/lib -L$(XERCES_LIB_DIR) -L$(VTK_LIB_DIR) -L$(OSG_LIB_DIR) -L$(PRODUCER_LIB_DIR) -lVJCinema -losg -losgDB -losgGA -losgUtil -losgText -losgFX	\
			-lxerces-c -lvtkRendering -lvtkIO -lvtkImaging  -lvtkCommon -lvtkFiltering -lvtkGenericFiltering -lvtkGraphics -lvtksys	\
			-lvtkfreetype -lvtkexpat -lvtkftgl -lvtkDICOMParser -lvtkpng -lvtktiff -lvtkzlib -lvtkjpeg -lvtkMPEG2Encode -lProducer
# Advanced options. 
#---------------------------------------------------------------

include $(VJ_BASE_DIR)/share/vrjuggler/vrj.appdefs.mk
include $(DZR_BASE_DIR)/ext/vrjuggler/dzr.vrjuggler.glapp.mk
include $(DZR_BASE_DIR)/ext/tweek/dzr.tweek.mk

#vpath %.cpp $(CXX_GEN_DIR)
#vpath %.h $(CXX_GEN_DIR)


# -----------------------------------------------------------------------------
# Application build targets.
# -----------------------------------------------------------------------------
all: 		
		$(MAKE) $(APP_NAME)

$(APP_NAME): 	$(OBJS)
		$(LINK) $(LINK_OUT)$@ $(OBJS) $(TWEEK_LIBS) $(EXTRA_LIBS) $(LIBS)


