# Set these variables for all platforms.
CPP = g++
GLOBAL_CPP_FLAGS = -D_DEBUG -g
#GLOBAL_CPP_FLAGS =
GLOBAL_LINK_FLAGS =

# Set these variables based on the operating system.
# Note: Need OS_TYPE in environment table.
# Note: When I figure out how to do if-elseif-else 
#			 statements I will change this.

# If we are on Irix.
ifeq ($(OS_TYPE), Irix)
	CPP = CC
	OS = IRIX
	Os = Irix
	os = irix
	PLATFORM_CPP_FLAGS = -LANG:std -D_SGI_NATIVE_COMPILER
	PLATFORM_LINK_FLAGS = -LANG:std -v
endif

# If we are on Linux. The flag '_ISOC99_SOURCE' is for vswprintf()
ifeq ($(OS_TYPE), Linux)
	CPP = g++
	OS = LINUX
	Os = Linix
	os = linux
	PLATFORM_CPP_FLAGS = -D_ISOC99_SOURCE
	PLATFORM_LINK_FLAGS =
endif
