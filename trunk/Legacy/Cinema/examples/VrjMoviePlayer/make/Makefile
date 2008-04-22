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

APP_NAME=	viewer


# Basic options.
#-------------------------------------------------------
BUILD_TYPE?=	opt

srcdir=		../src

SRCS=		MyApp.cpp	\
		main.cpp

NO_DEPEND=	YES
		
EXTRA_INCLUDES=		-I../include -I../../../ -I$(VRJCINEMA_ROOT_DIR)/src -I$(OSG_INC_DIR) -I$(OPENTHREADS_INC_DIR) -I$(PRODUCER_INC_DIR) \
			-I$(VJ_BASE_DIR)/include/tweek/idl -I$(VJ_BASE_DIR)/include -I$(VJ_BASE_DIR)/include/tweek/CORBA

EXTRA_LIBS=		-L$(VRJCINEMA_ROOT_DIR)/lib -L$(VRJCINEMA_ROOT_DIR)/bin -L$(OSG_LIB_DIR) -lVrjCinema -losg -losgDB -losgGA -losgUtil -losgText -losgFX -lCinema_s -L$(PRODUCER_LIB_DIR) -lProducer

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

