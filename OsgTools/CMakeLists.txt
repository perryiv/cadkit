
#########################################################
#
#  CMakeLists for OsgTools..
#
#########################################################

PROJECT(OsgTools)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

ADD_DEFINITIONS("-D_COMPILING_OSG_TOOLS")

# ------------ Find OpenGL Library ----------------------
FIND_PACKAGE(OpenGL)

# ------------ Find OpenSceneGraph Libraries ----------------------
INCLUDE ( ${CMakeModules}/FindOSG.cmake)

# ------------ Find Boost ----------------------
INCLUDE ( ${CMakeModules}/FindBoost.cmake)
# Note that the Boost Include path will automatically get added

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../" ${OSG_INC_DIR} ${OT_INC_DIR} )


#List the Sources
SET (SOURCES
  Animate/AnimationCallback.cpp
  Axes.cpp
  Builders/Arrow.cpp
  Builders/GradientBackground.cpp
  Callbacks/BoundingSphere.cpp
  Callbacks/SortBackToFront.cpp
  Circle.cpp
  ColorSetter.cpp
  DisplayLists.cpp
  Font.cpp
  GlassBoundingBox.cpp
  Grid.cpp
  Group.cpp
  Images/DownSample.cpp
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
  IO/Reader.cpp
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
  Render/EventAdapter.cpp
  Render/FBOScreenCapture.cpp
  Render/FrameDump.cpp
  Render/OffScreenRenderer.cpp
  Render/Renderer.cpp
  Render/SceneManager.cpp
  Render/TiledScreenCapture.cpp
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
  Utilities/ConvertToTriangles.cpp
  Utilities/DeleteHandler.cpp
  Utilities/FindNormals.cpp
  Utilities/Intersect.cpp
  Utilities/TranslateGeometry.cpp
  Widgets/Axes.cpp
  Widgets/ClipPlane.cpp
  Widgets/ProgressBar.cpp
  Widgets/ProgressBarGroup.cpp
  Widgets/GradientIcon.cpp
  Widgets/Item.cpp
  Widgets/Image.cpp
  Widgets/Legend.cpp
  Widgets/LegendObject.cpp
  Widgets/Text.cpp
  Volume/PlanarProxyGeometry.cpp
  Volume/Texture3DVolume.cpp
  Volume/TransferFunction.cpp
  Volume/TransferFunction1D.cpp
)

# Create a Shared Library
ADD_LIBRARY(OsgTools SHARED ${SOURCES} )

# Link the Library
LINK_CADKIT(OsgTools Usul)

TARGET_LINK_LIBRARIES(OsgTools
  ${Boost_LIBRARIES}
	${OPENGL_LIBRARIES}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSG_GA_LIB}
	${OSG_TEXT_LIB}
	${OSG_UTIL_LIB}
  ${OSG_MANIPULATOR}
  ${OSG_VIEWER}
)


INSTALL(TARGETS OsgTools
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin
  ARCHIVE DESTINATION bin
)
