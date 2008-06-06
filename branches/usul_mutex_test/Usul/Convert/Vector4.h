
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
//  String conversions for vector.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_CONVERT_STRING_VECTOR_4_H_
#define _USUL_CONVERT_STRING_VECTOR_4_H_

#include "Usul/Convert/Convert.h"
#include "Usul/IO/Vector4.h"


USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec4c );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec4s );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec4i );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec4l );

USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec4uc );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec4us );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec4ui );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec4ul );

USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec4f );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec4d );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec4ld );


#endif // _USUL_CONVERT_STRING_VECTOR_4_H_
