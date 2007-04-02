# Include the header to set up the environment.
include ${USUL_INC_DIR}/Usul/Make/header.mak

# C++ source files.
CPP_FILES = \
	GUI/Controller.cpp \
	Viz/Controller.cpp \
	Viz/Progress.cpp \
	DataObjects/Polygon.cpp \
	DataObjects/DataObject.cpp \
	DataObjects/Line.cpp \
	DataObjects/Point.cpp \
	DataObjects/PointTime.cpp \
	DB/Connection.cpp \
	Functors/SingleColorFunctor.cpp \
	Functors/GradientColorFunctor.cpp \
	Layers/Layer.cpp \
	Layers/LineLayer.cpp \
	Layers/PointLayer.cpp \
	Layers/PointTimeLayer.cpp \
	Layers/PolygonLayer.cpp \
	Layers/PolygonTimeLayer.cpp \
	Layers/RLayer.cpp \
	postGIS/BinaryParser.cpp \
	postGIS/Geometry.cpp \
	postGIS/Point.cpp \
	postGIS/Line.cpp \
	postGIS/Polygon.cpp \
	Scene/SceneManager.cpp

# The include path.
LOCAL_INCLUDE = \
	-I$(OSG_INC_DIR) \
	-I$(OPENTHREADS_INC_DIR) \
	-I$(OSSIM_INC_DIR) \
	-I$(OSSIM_PLANET_INC_DIR) \
	-I$(OSG_EPHEMERIS_INC_DIR) \
	-I$(BOOST_INC_DIR) \
	-I$(PQXX_INC_DIR) \
	-I$(CADKIT_ROOT_DIR) \
	-I$(CADKIT_ROOT_DIR)/Experimental \
	-I../../../../ \
	-I../../include

# The libraries to link to.
LOCAL_LIBS = \
	-L../../../../bin -lOsgTools -lUsul -lXmlTree -lXmlSerialize \
	-L$(OSSIM_LIB_DIR) -lossim \
	-L$(OSSIMPLANET_LIB_DIR) -lossimPlanet \
	-L$(PQXX_LIB_DIR) -lpqxx \
	-L$(BOOST_LIB_DIR) \
	-lboost_date_time-gcc-mt-1_33_1 \
	-lboost_serialization-gcc-mt-1_33_1


# Any flags for the compiler. 
LOCAL_CPP_FLAGS = 

# The name of the thing we are making.
TARGET = Minerva

# The common rules for compiling are in here.
include ${USUL_INC_DIR}/Usul/Make/library.mak
