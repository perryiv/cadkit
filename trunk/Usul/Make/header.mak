# Set these variables for all platforms.
CPP = g++

DEBUG_CPP_FLAGS = -D_DEBUG -g

GLOBAL_CPP_FLAGS =
GLOBAL_LINK_FLAGS =

PLATFORM_CPP_FLAGS =
PLATFORM_LINK_FLAGS =
PLATFORM_INCLUDE_FLAGS =

EXECUTABLE_EXTENSION =

# Append target names with a 'd' if this is a debug build.
ifdef ($_DEBUG)
	TARGET_SUFFIX = d
endif

# Set these variables based on the operating system.
# Note: Need OS_TYPE in environment table.

# If we are on Irix.
ifeq ($(OS_TYPE), Irix)
	CPP = CC
	OS = IRIX
	Os = Irix
	os = irix
	PLATFORM_CPP_FLAGS = -LANG:std
	PLATFORM_LINK_FLAGS = -LANG:std -v
endif

# If we are on Linux. The flag '_ISOC99_SOURCE' is for vswprintf()
ifeq ($(OS_TYPE), Linux)
	OS = LINUX
	Os = Linix
	os = linux
	PLATFORM_CPP_FLAGS = -D_ISOC99_SOURCE
endif

# If we are on Cygwin.
ifeq ($(OS_TYPE), Cygwin)
	OS = CYGWIN
	Os = Cygwin
	os = cygwin
	EXECUTABLE_EXTENSION = .exe
endif
