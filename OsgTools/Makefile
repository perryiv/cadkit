
# Include the header to set up the environment.
include ${CADKIT_MAKE_DIR}/header.mak

# C++ source files.
CPP_FILES = \
  ./Builders/Sign.cpp \
  ./Builders/Arrow.cpp \
  ./Builders/GradientBackground.cpp \
  ./Builders/Compass.cpp \
  ./ShapeFactory.cpp \
  ./Animate/AnimationCallback.cpp \
  ./Triangles/Factory.cpp \
  ./Triangles/SharedVertex.cpp \
  ./Triangles/LoopSplitter.cpp \
  ./Triangles/Loop.cpp \
  ./Triangles/Triangle.cpp \
  ./Triangles/Blocks.cpp \
  ./Triangles/Block.cpp \
  ./Triangles/ColorFunctor.cpp \
  ./Triangles/TriangleSet.cpp \
  ./Lod.cpp \
  ./Axes.cpp \
  ./Statistics.cpp \
  ./MaterialFactory.cpp \
  ./Widgets/Axes.cpp \
  ./Widgets/ClipPlane.cpp \
  ./Widgets/Helper/ThreadSafeText.cpp \
  ./Widgets/Helper/UpdateProgress.cpp \
  ./Widgets/ProgressBar.cpp \
  ./Widgets/ProgressBarGroup.cpp \
  ./Torus.cpp \
  ./MaterialSetter.cpp \
  ./Draggers/Translate.cpp \
  ./Draggers/Callback.cpp \
  ./Draggers/Rotate1.cpp \
  ./Draggers/Plane.cpp \
  ./Draggers/Translate3.cpp \
  ./Draggers/Trackball.cpp \
  ./Draggers/TranslateScreen.cpp \
  ./Draggers/Dragger.cpp \
  ./Draggers/Translate1.cpp \
  ./Draggers/Translate2.cpp \
  ./Intersect/FrustumVisitor.cpp \
  ./Ray.cpp \
  ./Font.cpp \
  ./Text.cpp \
  ./Volume/PlanarProxyGeometry.cpp \
  ./Volume/TransferFunction1D.cpp \
  ./Volume/Texture3DVolume.cpp \
  ./Volume/GPURayCasting.cpp \
  ./Volume/TransferFunction.cpp \
  ./Mesh.cpp \
  ./Callbacks/BoundingSphere.cpp \
  ./Callbacks/SortBackToFront.cpp \
  ./State/StateSet.cpp \
  ./State/PolygonMode.cpp \
  ./State/ShadeModel.cpp \
  ./Images/Scale.cpp \
  ./Images/Invert.cpp \
  ./Images/Threshold.cpp \
  ./Images/DownSample.cpp \
  ./Images/Morphology.cpp \
  ./Images/Histogram.cpp \
  ./Images/ImageOr.cpp \
  ./Images/Image3d.cpp \
  ./Images/EdgeDetect.cpp \
  ./Images/Grayscale.cpp \
  ./Render/FBOScreenCapture.cpp \
  ./Render/EventAdapter.cpp \
  ./Render/FrameDump.cpp \
  ./Render/Animation.cpp \
  ./Render/ClipPlaneCallback.cpp \
  ./Render/LightCallback.cpp \
  ./Render/Trackball.cpp \
  ./Render/TiledScreenCapture.cpp \
  ./Render/SceneManager.cpp \
  ./Render/Renderer.cpp \
  ./Render/Viewer.cpp \
  ./Render/ActionAdapter.cpp \
  ./IO/Reader.cpp \
  ./IO/STLWriter.cpp \
  ./IO/GLFeedback.cpp \
  ./IO/WriteEPS.cpp \
  ./ReadModel.cpp \
  ./Group.cpp \
  ./Grid.cpp \
  ./Drawables/VoxelGeometry.cpp \
  ./Circle.cpp \
  ./Legend/GradientIcon.cpp \
  ./Legend/SolidIcon.cpp \
  ./Legend/LegendObject.cpp \
  ./Legend/Legend.cpp \
  ./Legend/Text.cpp \
  ./Legend/Icon.cpp \
  ./GlassBoundingBox.cpp \
  ./Jitter.cpp \
  ./ColorSetter.cpp \
  ./Utilities/FindNormals.cpp \
  ./Utilities/Intersect.cpp \
  ./DisplayLists.cpp \
  ./Sample.cpp 

# The include path.
LOCAL_INCLUDE = \
	-I../ \
	-I$(OSG_INC_DIR) \
	-I$(CADKIT_INC_DIR)

# The name of the thing we are making.
TARGET = OsgTools

include ${CADKIT_MAKE_DIR}/library.mak
