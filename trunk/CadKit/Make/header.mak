# Set these variables based on the operating system.
# Note: need OS_TYPE in environment table.
ifeq ($(OS_TYPE), Irix)
	CPP = CC
	OS = IRIX
	os = irix
	Os = Irix
	GLOBAL_CPP_FLAGS = -LANG:std -D_SGI_NATIVE_COMPILER -D_DEBUG
else
	CPP = g++
	GLOBAL_CPP_FLAGS = -D_DEBUG
endif
