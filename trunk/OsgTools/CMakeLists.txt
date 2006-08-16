
PROJECT(OsgTools)

SET (CMAKE_BUILD_TYPE Debug)
SET (CMAKE_SKIP_RPATH OFF)

#Setup output Directories
SET (LIBRARY_OUTPUT_PATH
${PROJECT_SOURCE_DIR}/../Bin/${CMAKE_BUILD_TYPE}
CACHE PATH
"Single Directory for all Libraries"
)

#Setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
${PROJECT_SOURCE_DIR}/../Bin/${CMAKE_BUILD_TYPE}
CACHE PATH
"SINGLE Directory for all Executables."
)


# ------------ Find Usul Library ----------------------
FIND_LIBRARY (USUL_LIBRARY Usul
	${PROJECT_SOURCE_DIR}/../Bin/${CMAKE_BUILD_TYPE}
	/usr/local/lib
)

# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)

# ------------ Find OpenThreads Library ----------------------
FIND_LIBRARY(OPENTHREADS_LIB OpenThreads
	/usr/local/lib
	/usr/lib
)
# ------------ Find osg Library ----------------------
FIND_LIBRARY(OSG_LIB osg
	/usr/local/lib
	/usr/lib
)
# ------------ Find osgDB Library ----------------------
FIND_LIBRARY(OSG_DB_LIB osgDB
	/usr/local/lib
	/usr/lib
)
# ------------ Find osgGA Library ----------------------
FIND_LIBRARY(OSG_GA_LIB osgGA
	/usr/local/lib
	/usr/lib
)
# ------------ Find osgText Library ----------------------
FIND_LIBRARY(OSG_TEXT_LIB osgText
	/usr/local/lib
	/usr/lib
)
# ------------ Find osgUtil Library ----------------------
FIND_LIBRARY(OSG_UTIL_LIB osgUtil
	/usr/local/lib
	/usr/lib
)


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../" "$ENV{OSG_INC_DIR}" "$ENV{OPENTHREADS_INC_DIR}" )

LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/../Bin/${CMAKE_BUILD_TYPE} "$ENV{OSG_LIB_DIR}" "$ENV{OPENTHREADS_LIB_DIR}" )

#List the Sources
SET (SOURCES
	Axes.cpp
	Builders/Arrow.cpp
	Callbacks/BoundingSphere.cpp
	Callbacks/SortBackToFront.cpp
	Circle.cpp
	ColorSetter.cpp
	DisplayLists.cpp
	Draggers/Callback.cpp
	Draggers/Dragger.cpp
	Draggers/Plane.cpp
	Draggers/Rotate1.cpp
	Draggers/Trackball.cpp
	Draggers/Translate.cpp
	Draggers/Translate1.cpp
	Draggers/Translate2.cpp
	Draggers/Translate3.cpp
	Draggers/TranslateScreen.cpp
	Drawables/VoxelGeometry.cpp
	Font.cpp
	GlassBoundingBox.cpp
	Grid.cpp
	Group.cpp
	Images/EdgeDetect.cpp
	Images/Grayscale.cpp
	Images/Histogram.cpp
	Images/Image3d.cpp
	Images/ImageOr.cpp
	Images/Invert.cpp
	Images/Morphology.cpp
	Images/Scale.cpp
	Images/Threshold.cpp
	Intersect/FrustumVisitor.cpp
	IO/GLFeedback.cpp
	IO/STLWriter.cpp
	IO/WriteEPS.cpp
	Jitter.cpp
	Lod.cpp
	MaterialFactory.cpp
	MaterialSetter.cpp
	Mesh.cpp
	Ray.cpp
	ReadModel.cpp
	Render/ActionAdapter.cpp
	Render/Animation.cpp
	Render/ClipPlaneCallback.cpp
	Render/EventAdapter.cpp
	Render/FrameDump.cpp
	Render/LightCallback.cpp
	Render/Renderer.cpp
	Render/SceneManager.cpp
	Render/Trackball.cpp
	Render/Viewer.cpp
	Sample.cpp
	ShapeFactory.cpp
	State/PolygonMode.cpp
	State/ShadeModel.cpp
	State/StateSet.cpp
	Statistics.cpp
	Text.cpp
	Torus.cpp
	Triangles/Block.cpp
	Triangles/Blocks.cpp
	Triangles/ColorFunctor.cpp
	Triangles/Factory.cpp
	Triangles/Loop.cpp
	Triangles/LoopSplitter.cpp
	Triangles/SharedVertex.cpp
	Triangles/Triangle.cpp
	Triangles/TriangleSet.cpp
	Utilities/FindNormals.cpp
	Utilities/Intersect.cpp
	Widgets/Axes.cpp
	Widgets/ClipPlane.cpp
)

# Create a Shared Library
ADD_LIBRARY(OsgTools SHARED ${SOURCES} )

# Link the Library	
TARGET_LINK_LIBRARIES(OsgTools 
	Usul
	OpenThreads
	osg
	osgDB
	osgGA
	osgText
	osgUtil
	${OPENGL_LIBRARIES}
	)

    
    