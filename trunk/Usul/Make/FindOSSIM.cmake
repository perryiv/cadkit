
# Set variables in Cmake.  Use the user set environment variables
# If these are not set, then they will simply be ignored.

# ------------ Wms variables ----------------------
 set(WMS_INC_DIR "$ENV{LIBWMS_INC_DIR}" )
 set(WMS_LIB_DIR "$ENV{LIBWMS_LIB_DIR}" )


# ------------ Ossim variables ----------------------
 set(OSSIM_LIB_DIR "$ENV{OSSIM_LIB_DIR}")
 set(OSSIM_INC_DIR "$ENV{OSSIM_INC_DIR}")

# ------------ OssimPlanet variables ---------------------- 

 set( OSSIM_PLANET_INC_DIR "$ENV{OSSIM_PLANET_INC_DIR}" )
 set( OSSIM_PLANET_LIB_DIR "$ENV{OSSIM_PLANET_LIB_DIR}" )

# ------------ OsgEphemeris variables ---------------------- 

 set ( OSG_EPHEMERIS_INC_DIR "$ENV{OSG_EPHEMERIS_INC_DIR}" )
 set ( OSG_EPHEMERIS_LIB_DIR "$ENV{OSG_EPHEMERIS_LIB_DIR}" )



# ------------ Find wms Library ----------------------
FIND_LIBRARY(WMS_LIB wms ${WMS_LIB_DIR} )

# ------------ Find Ossim Library --------------------
FIND_LIBRARY(OSSIM_LIB ossim ${OSSIM_LIB_DIR} )

# ------------ Find OssimPlanet Library --------------------
FIND_LIBRARY(OSSIM_PLANET ossimPlanet ${OSSIM_PLANET_LIB_DIR} )

# ------------ Find OssimPlanet Library --------------------
FIND_LIBRARY(OSG_EPHEMERIS_LIB osgEphemeris ${OSG_EPHEMERIS_LIB_DIR} )
