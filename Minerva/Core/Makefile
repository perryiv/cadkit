# Include the header to set up the environment.
include header.mak

# C++ source files.
CPP_FILES = \
	../../src/Minerva/Core/GUI/Controller.cpp \
	../../src/Minerva/Core/Viz/Controller.cpp \
	../../src/Minerva/Core/Viz/Progress.cpp \
	../../src/Minerva/DataObjects/Boundary.cpp \
	../../src/Minerva/DataObjects/DataObject.cpp \
	../../src/Minerva/DataObjects/Line.cpp \
	../../src/Minerva/DataObjects/Point.cpp \
	../../src/Minerva/DataObjects/PointTime.cpp \
	../../src/Minerva/DB/Connection.cpp \
	../../src/Minerva/Layers/Functors/SingleColorFunctor.cpp \
	../../src/Minerva/Layers/Functors/GradientColorFunctor.cpp \
	../../src/Minerva/Layers/Layer.cpp \
	../../src/Minerva/Layers/LineLayer.cpp \
	../../src/Minerva/Layers/PointLayer.cpp \
	../../src/Minerva/Layers/PointTimeLayer.cpp \
	../../src/Minerva/Layers/PolygonLayer.cpp \
	../../src/Minerva/Layers/PolygonTimeLayer.cpp \
	../../src/Minerva/Layers/RLayer.cpp \
	../../src/Minerva/postGIS/BinaryParser.cpp \
	../../src/Minerva/postGIS/Triangulate.cpp \
	../../src/Minerva/postGIS/Geometry.cpp \
	../../src/Minerva/postGIS/Point.cpp \
	../../src/Minerva/postGIS/Line.cpp \
	../../src/Minerva/postGIS/Polygon.cpp \
	../../src/Minerva/Scene/SceneManager.cpp

# The include path.
LOCAL_INCLUDE = \
	-I$(OSG_INC_DIR) \
	-I$(OPENTHREADS_INC_DIR) \
	-I$(OSSIM_INC_DIR) \
	-I$(OSSIM_PLANET_INC_DIR) \
	-I$(OSG_EPHEMERIS_INC_DIR) \
	-I$(BOOST_INC_DIR) \
	-I$(PQXX_INC_DIR) \
	-I$(CGAL_INC_DIR) \
	-I$(CGAL_INC_DIR)/CGAL/config/x86-64_Linux-2.6_g++-3.4.4 \
	-I$(CADKIT_ROOT_DIR)/Magrathea/src \
	-I../../../../ \
	-I../../include

# The libraries to link to.
LOCAL_LIBS = \
	-L../../../../bin -lOsgTools -lUsul \
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
include library.mak
