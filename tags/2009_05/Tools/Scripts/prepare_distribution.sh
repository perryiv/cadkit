#!/bin/tcsh

setenv TARGET ${HOME}/distributions/`date "+%Y_%m_%d"`
mkdir -p ${TARGET}

#setenv PREPARE ${CADKIT_INC_DIR}/Tools/PrepareDistribution/PrepareDistribution/Debug/PrepareDistribution
setenv PREPARE ${CADKIT_BIN_DIR}/PrepareDistribution
${PREPARE}

cd ${OSG_LIB_DIR}/osgPlugins-2.2.0
foreach i (*.so)
${PREPARE} $i ${TARGET}
end

cd ${CADKIT_BIN_DIR}
foreach i (*.so)
${PREPARE} $i ${TARGET}
end

cd ${CADKIT_BIN_DIR}
foreach i (*.plug)
${PREPARE} $i ${TARGET}
end
