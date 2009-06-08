



# TODO.  I'd really like to have the CGAL_CONFIG_INC_DIR found without 
# the environment variable.

set( CGAL_INC_DIR $ENV{CGAL_INC_DIR} )
set( CGAL_CONFIG_INC_DIR $ENV{CGAL_CONFIG_INC_DIR} )


FIND_LIBRARY( CGAL_LIB CGAL $ENV{CGAL_LIB_DIR} )
