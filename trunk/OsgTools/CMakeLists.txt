
PROJECT(OsgTools)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")

# If the user has defined OSGTOOLS_BIN_DIR as an environment variable
#  then use that directory as the output directory. Otherwise set the output
#  directory to be inside the current build directory.
#  First check to see if it was defined in another project

# This will be true if the user has NOT defined an environment variable
# Set directory to be same as Usul's
IF ( NOT OSGTOOLS_BIN_DIR )
SET(OSGTOOLS_BIN_DIR "$ENV{OSGTOOLS_BIN_DIR}" )
ENDIF ( NOT OSGTOOLS_BIN_DIR )

IF ( NOT OSGTOOLS_BIN_DIR ) 
SET(OSGTOOLS_BIN_DIR  ${PROJECT_BINARY_DIR}/Bin )
ENDIF ( NOT OSGTOOLS_BIN_DIR )


# setup output Directories
SET (LIBRARY_OUTPUT_PATH
	${OSGTOOLS_BIN_DIR}
	CACHE PATH
	"Single Directory for all Libraries"
)

# setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
	${OSGTOOLS_BIN_DIR}
	CACHE PATH
	"Single Directory for all Executables."
)


# Make sure Usul is built
SUBDIRS (${PROJECT_SOURCE_DIR}/../Usul)

# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE ( ${CMakeModules}/FindOSG.cmake)

# ------------ Find Boost ----------------------
SET (Boost_USE_DATE_TIME TRUE)
INCLUDE ( ${CMakeModules}/FindBOOST.cmake)
# Note that the Boost Include path will automatically get added


# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../" ${OSG_INC_DIR} ${OT_INC_DIR} )


#List the Sources
SET (SOURCES
  Animate/AnimationCallback.cpp
  Animate/Date.cpp
  Animate/DateCallback.cpp
  Animate/DateGroup.cpp
  Animate/Settings.cpp
  Axes.cpp
  Builders/Arrow.cpp
  Builders/GradientBackground.cpp
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
  Legend/GradientIcon.cpp
  Legend/Icon.cpp
  Legend/Legend.cpp
  Legend/LegendObject.cpp
  Legend/SolidIcon.cpp
  Legend/Text.cpp
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
  ${Boost_LIBRARIES}
	${OPENGL_LIBRARIES}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSG_GA_LIB}
	${OSG_TEXT_LIB}
	${OSG_UTIL_LIB}
)

    
    
