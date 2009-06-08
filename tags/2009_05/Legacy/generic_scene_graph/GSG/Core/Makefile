# Include the header to set up the environment.
include $(GSG_ROOT_DIR)/Make/header.mak

# C++ source files.
CPP_FILES = \
	Attributes.cpp \
	CoreDll.cpp \
	FrustumCull.cpp \
	Draw.cpp \
	Factory.cpp \
	Group.cpp \
	Lod.cpp \
	Node.cpp \
	Path.cpp \
	Primitive.cpp \
	Referenced.cpp \
	Shape.cpp \
	Transform.cpp \
	Update.cpp \
	Vec2Pool.cpp \
	Vec3Pool.cpp \
	Vec4Pool.cpp \
	Visitor.cpp

# The include path.
LOCAL_INCLUDE =

# The libraries to link to.
LOCAL_LIBS = 

# Any flags for the compiler.
LOCAL_CPP_FLAGS = -DCOMPILING_GSG_CORE

# The name of the thing we are making.
TARGET = gsg

# The common rules for compiling are in here.
include $(GSG_ROOT_DIR)/Make/library.mak
