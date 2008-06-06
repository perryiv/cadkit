
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  String converter for osg::Matrix.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_STRING_MATRIX_CONVERSION_H_
#define _OSG_TOOLS_STRING_MATRIX_CONVERSION_H_

#include "Usul/Convert/Convert.h"

#include "osg/Matrixd"


USUL_CONVERT_DEFINE_STRING_CONVERTER_MATRIX_4_4 ( osg::Matrixd );
USUL_CONVERT_DEFINE_STRING_CONVERTER_MATRIX_4_4 ( osg::Matrixf );


#endif // _OSG_TOOLS_STRING_MATRIX_CONVERSION_H_
