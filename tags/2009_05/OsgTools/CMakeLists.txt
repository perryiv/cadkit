
#########################################################
#
#  CMakeLists for OsgTools.
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

LINK_DIRECTORIES( ${CADKIT_BIN_DIR} )

# ------------ Set Include Folders ----------------------
#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../" ${Boost_INCLUDE_DIR} ${OSG_INC_DIR} ${OT_INC_DIR} )

#########################################################
#
#  List the Sources.
#
#########################################################

SET ( HEADERS
	./Axes.h
	./Box.h
	./Builders/Arrow.h
	./Builders/GradientBackground.h
	./Callbacks/SortBackToFront.h
	./Circle.h
	./ColorPolicyFunctor.h
	./ColorSetter.h
	./Configure/OSG.h
	./Convert/Matrix.h
	./Convert/MatrixTransform.h
	./Convert.h
	./Declarations.h
	./Dimension.h
	./DisplayLists.h
	./Export.h
	./FlipNormals.h
	./Font.h
	./ForEach.h
	./GlassBoundingBox.h
	./GraphStreamVisitor.h
	./Grid.h
	./Group.h
	./HasOption.h
	./Images/DownSample.h
	./Images/Image3d.h
	./Index.h
	./IO/GLFeedback.h
	./IO/Reader.h
	./IO/STLPrintVisitor.h
	./IO/STLWrite.h
	./IO/STLWriter.h
	./Jitter.h
	./KeepFirst.h
	./Lod.h
	./MaterialFactory.h
	./MaterialSetter.h
	./Mesh.h
	./Objects/Cylinder.h
	./Objects/Macros.h
	./Objects/Object.h
	./Objects/RegularGrid.h
	./Objects/VertexSequence.h
	./Points/OctTree.h
	./Points/OctTreeNode.h
	./Points/PointLoader.h
	./Points/PointSet.h
	./Points/PointSetRecords.h
	./Ray.h
	./Render/ActionAdapter.h
	./Render/Animation.h
	./Render/ClampProjection.h
	./Render/ClipPlaneCallback.h
	./Render/Constants.h
	./Render/Defaults.h
	./Render/EventAdapter.h
	./Render/FBOScreenCapture.h
	./Render/FrameDump.h
	./Render/LodCallbacks.h
	./Render/OffScreenRenderer.h
	./Render/RecordTime.h
	./Render/Renderer.h
	./Render/SceneManager.h
	./Render/TiledScreenCapture.h
	./Render/Trackball.h
	./Render/Viewer.h
	./Sample.h
	./ScopedDisplayLists.h
	./ScopedNodeMask.h
	./ScopedOptions.h
	./ScopedProjection.h
	./ScopedViewport.h
	./SetDataVariance.h
	./ShapeFactory.h
	./Sphere.h
	./State/PolygonMode.h
	./State/ShadeModel.h
	./State/StateSet.h
	./Statistics.h
	./Text.h
	./Torus.h
	./Triangles/Block.h
	./Triangles/Blocks.h
	./Triangles/ColorFunctor.h
	./Triangles/Constants.h
	./Triangles/Enum.h
	./Triangles/Exceptions.h
	./Triangles/Factory.h
	./Triangles/Loop.h
	./Triangles/LoopSplitter.h
	./Triangles/SharedVertex.h
	./Triangles/Triangle.h
	./Triangles/TriangleSet.h
	./Utilities/ConvertToTriangles.h
	./Utilities/DeleteHandler.h
	./Utilities/DirtyBounds.h
	./Utilities/FindNormals.h
	./Utilities/Intersect.h
	./Utilities/ReferenceFrame.h
	./Utilities/TranslateGeometry.h
	./Utilities/Transparency.h
	./Visitor.h
	./Volume/GPURayCasting.h
	./Volume/PlanarProxyGeometry.h
	./Volume/Texture3DVolume.h
	./Volume/TransferFunction.h
	./Volume/TransferFunction1D.h
	./Widgets/Axes.h
	./Widgets/ClipPlane.h
	./Widgets/GradientIcon.h
	./Widgets/Image.h
	./Widgets/Item.h
	./Widgets/Legend.h
	./Widgets/LegendObject.h
	./Widgets/LineFunctor.h
	./Widgets/ProgressBar.h
	./Widgets/ProgressBarGroup.h
	./Widgets/QuadFunctor.h
	./Widgets/SolidIcon.h
	./Widgets/SphereFunctor.h
	./Widgets/Text.h
)

#########################################################
#
#  List the Sources.
#
#########################################################

SET (SOURCES
  Axes.cpp
  Builders/Arrow.cpp
  Builders/GradientBackground.cpp
  Callbacks/SortBackToFront.cpp
  Circle.cpp
  ColorSetter.cpp
  DisplayLists.cpp
  Font.cpp
  GlassBoundingBox.cpp
  Grid.cpp
  Group.cpp
  Images/DownSample.cpp
  Images/Image3d.cpp
  IO/GLFeedback.cpp
  IO/Reader.cpp
  IO/STLWriter.cpp
  Jitter.cpp
  Lod.cpp
  MaterialFactory.cpp
  MaterialSetter.cpp
  Mesh.cpp
  Ray.cpp
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
  Volume/GPURayCasting.cpp
  Volume/PlanarProxyGeometry.cpp
  Volume/Texture3DVolume.cpp
  Volume/TransferFunction.cpp
  Volume/TransferFunction1D.cpp
)

#########################################################
#
#  Add Source groups.
#
#########################################################

SOURCE_GROUP ( Builders  REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Builders/* )
SOURCE_GROUP ( Callbacks REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Callbacks/* )
SOURCE_GROUP ( Configure REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Configure/* )
SOURCE_GROUP ( Convert   REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Convert/* )
SOURCE_GROUP ( Images    REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Images/* )
SOURCE_GROUP ( IO        REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/IO/* )
SOURCE_GROUP ( Objects   REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Objects/* )
SOURCE_GROUP ( Render    REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Render/* )
SOURCE_GROUP ( State     REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/State/* )
SOURCE_GROUP ( Triangles REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Triangles/* )
SOURCE_GROUP ( Utilities REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Utilities/* )
SOURCE_GROUP ( Widgets   REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Widgets/* )
SOURCE_GROUP ( Volume    REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Volume/* )

# Set the target name.
SET ( TARGET_NAME OsgTools )
SET ( TARGET_FILES ${HEADERS} ${SOURCES} )

# Create a Shared Library
ADD_LIBRARY(${TARGET_NAME} SHARED ${TARGET_FILES} )

# Add a library
CADKIT_ADD_LIBRARY ( ${TARGET_NAME} )

# Link to these Cadkit libraries.
LINK_CADKIT(${TARGET_NAME} Usul)

# Link to these 3rd party librarise.
TARGET_LINK_LIBRARIES( ${TARGET_NAME}
	${Boost_LIBRARIES}
	${OPENGL_LIBRARIES}
	${OPENTHREADS_LIB}
	${OSG_LIB}
	${OSG_DB_LIB}
	${OSG_GA_LIB}
	${OSG_TEXT_LIB}
	${OSG_UTIL_LIB}
	${OSG_MANIPULATOR_LIB}
	${OSG_VIEWER_LIB}
)
