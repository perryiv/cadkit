
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

#ifndef _USUL_CONVERT_STRING_VECTOR_2_H_
#define _USUL_CONVERT_STRING_VECTOR_2_H_

#include "Usul/Convert/Convert.h"
#include "Usul/IO/Vector2.h"


USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec2c );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec2s );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec2i );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec2l );

USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec2uc );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec2us );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec2ui );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec2ul );

USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec2f );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec2d );
USUL_CONVERT_DEFINE_STRING_CONVERTER ( Usul::Math::Vec2ld );


#endif // _USUL_CONVERT_STRING_VECTOR_2_H_
