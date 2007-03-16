
set(PRODUCER_LIB_DIR "$ENV{PRODUCER_LIB_DIR}")
set(PRODUCER_INC_DIR "$ENV{PRODUCER_INC_DIR}")
 

# ------------ Find Producer Library ----------------------
FIND_LIBRARY(PRODUCER_LIB Producer ${PRODUCER_LIB_DIR} )