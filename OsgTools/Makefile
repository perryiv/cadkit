# Include the header to set up the environment.
include ${USUL_INC_DIR}/Usul/Make/header.mak


# C++ source files.
CPP_FILES = \
./Animate/AnimationCallback.cpp \
./Animate/Date.cpp \
./Animate/DateCallback.cpp \
./Animate/DateGroup.cpp \
./Animate/Settings.cpp \
./Axes.cpp \
./Builders/Arrow.cpp \
./Builders/GradientBackground.cpp \
./Builders/Sign.cpp \
./Callbacks/BoundingSphere.cpp \
./Callbacks/SortBackToFront.cpp \
./Circle.cpp \
./ColorSetter.cpp \
./DisplayLists.cpp \
./Draggers/Callback.cpp \
./Draggers/Dragger.cpp \
./Draggers/Plane.cpp \
./Draggers/Rotate1.cpp \
./Draggers/Trackball.cpp \
./Draggers/Translate.cpp \
./Draggers/Translate1.cpp \
./Draggers/Translate2.cpp \
./Draggers/Translate3.cpp \
./Draggers/TranslateScreen.cpp \
./Drawables/VoxelGeometry.cpp \
./Font.cpp \
./GlassBoundingBox.cpp \
./Grid.cpp \
./Group.cpp \
./Images/EdgeDetect.cpp \
./Images/Grayscale.cpp \
./Images/Histogram.cpp \
./Images/Image3d.cpp \
./Images/ImageOr.cpp \
./Images/Invert.cpp\ 
./Images/Morphology.cpp \
./Images/Scale.cpp \
./Images/Threshold.cpp \
./Intersect/FrustumVisitor.cpp \
./IO/GLFeedback.cpp \
./IO/Reader.cpp \
./IO/STLWriter.cpp \
./IO/WriteEPS.cpp \
./Jitter.cpp \
./Legend/GradientIcon.cpp \
./Legend/Icon.cpp \
./Legend/Legend.cpp \
./Legend/LegendObject.cpp \
./Legend/SolidIcon.cpp \
./Legend/Text.cpp \
./Lod.cpp \
./MaterialFactory.cpp \
./MaterialSetter.cpp \
./Mesh.cpp \
./Ray.cpp \
./ReadModel.cpp \
./Render/ActionAdapter.cpp \
./Render/Animation.cpp \
./Render/ClipPlaneCallback.cpp \
./Render/EventAdapter.cpp \
./Render/FrameDump.cpp \
./Render/LightCallback.cpp \
./Render/Renderer.cpp \
./Render/SceneManager.cpp \
./Render/Trackball.cpp \
./Render/Viewer.cpp \
./Sample.cpp \ 
./ShapeFactory.cpp \
./State/PolygonMode.cpp \
./State/ShadeModel.cpp \
./State/StateSet.cpp \
./Statistics.cpp \
./Text.cpp \
./Torus.cpp \
./Triangles/Block.cpp \
./Triangles/Blocks.cpp \
./Triangles/ColorFunctor.cpp \
./Triangles/Factory.cpp \
./Triangles/Loop.cpp \
./Triangles/LoopSplitter.cpp \
./Triangles/SharedVertex.cpp \
./Triangles/Triangle.cpp\ 
./Triangles/TriangleSet.cpp \
./Utilities/FindNormals.cpp \
./Utilities/Intersect.cpp \
./Widgets/Axes.cpp \
./Widgets/ClipPlane.cpp \


# The include path.
LOCAL_INCLUDE = \
	-I../ \
	-I$(OSG_INC_DIR) \
	-I$(USUL_INC_DIR) \
	-I$(OPENTHREADS_INC_DIR)

# The libraries to link to.
LOCAL_LIBS = \
	-L$(OSG_LIB_DIR) -L$(CADKIT_ROOT_DIR)/bin -L$(OPENTHREADS_LIB_DIR) \
	 -losg -losgUtil -losgDB -losgText -lUsul -lOpenThreads

# The name of the thing we are making.
TARGET = OsgTools	

include ${USUL_INC_DIR}/Usul/Make/library.mak
