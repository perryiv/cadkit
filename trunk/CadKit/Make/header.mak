# Set these variables as the defaults.
CPP = g++
GLOBAL_CPP_FLAGS = -D_DEBUG
GLOBAL_LINK_FLAGS =

# Set these variables based on the operating system, redefining some from above.
# Note: Need OS_TYPE in environment table.
# Note: When I figure out how to do if-elseif-else statements I will change this.

# If we are on Irix...
ifeq ($(OS_TYPE), Irix)
  CPP = CC
  OS = IRIX
  Os = Irix
  os = irix
  GLOBAL_CPP_FLAGS = -LANG:std -D_SGI_NATIVE_COMPILER -D_DEBUG
  GLOBAL_LINK_FLAGS = -LANG:std
endif

# If we are on Linux...
ifeq ($(OS_TYPE), Linux)
  CPP = g++
  OS = LINUX
  Os = Linix
  os = linux
  GLOBAL_CPP_FLAGS = -D_DEBUG
  GLOBAL_LINK_FLAGS =
endif
