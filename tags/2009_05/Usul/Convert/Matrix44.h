
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Perry Miller
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  String conversions for matrix.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_CONVERT_STRING_MATRIX_4_4_H_
#define _USUL_CONVERT_STRING_MATRIX_4_4_H_

#include "Usul/Convert/Convert.h"
#include "Usul/IO/Matrix44.h"


USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Matrix44c );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Matrix44s );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Matrix44i );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Matrix44l );

USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Matrix44uc );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Matrix44us );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Matrix44ui );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Matrix44ul );

USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Matrix44f );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Matrix44d );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Matrix44ld );


#endif // _USUL_CONVERT_STRING_MATRIX_4_4_H_
