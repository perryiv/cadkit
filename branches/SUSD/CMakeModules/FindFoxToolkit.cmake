

# Pick up if the user has set an environment variable to point to the 
# FoxToolkit Lib and includes. If these are not set
# then they will simply be ignored.
set(FOX_INC_DIR "$ENV{FOX_INC_DIR}")
set(FOX_LIB_DIR "$ENV{FOX_LIB_DIR}")

# ------------ Find FoxToolkit Library and include Dir ----------------------
FIND_LIBRARY(FOXTOOLKIT_LIB FOX-1.4
	${FOX_LIB_DIR}
)

FIND_PATH(FOXTOOLKIT_INCLUDE fx.h
    /usr/local/include/fox-1.4
    ${FOX_INC_DIR}
)

