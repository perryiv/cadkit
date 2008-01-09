
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

#ifndef _USUL_CONVERT_STRING_VECTOR_3_H_
#define _USUL_CONVERT_STRING_VECTOR_3_H_

#include "Usul/Convert/Convert.h"
#include "Usul/IO/Vector3.h"


USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec3c );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec3s );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec3i );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec3l );

USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec3uc );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec3us );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec3ui );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec3ul );

USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec3f );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec3d );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec3ld );


#endif // _USUL_CONVERT_STRING_VECTOR_3_H_
