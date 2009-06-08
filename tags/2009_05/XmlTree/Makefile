
# Include all variables, flags, etc...
include ${CADKIT_MAKE_DIR}/header.mak

# C++ source files.
CPP_FILES = \
  ./Detail/RootImpl.cpp \
  ./RegistryVisitor.cpp \
  ./Document.cpp \
  ./RegistryIO.cpp \
  ./Loader.cpp \
  ./ReplaceIllegalCharacters.cpp \
  ./Node.cpp \
  ./Writer.cpp \
  ./XercesLife.cpp \
  ./Functions.cpp \
  ./RegistryBuilder.cpp

LOCAL_INCLUDE = \
	-I../

# The name of the thing we are making.
TARGET = XmlTree

# Include the rules for make...
include ${CADKIT_MAKE_DIR}/library.mak
