
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
//  Text output for vector.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_IO_TEXT_WRITER_VECTOR_2_H_
#define _USUL_IO_TEXT_WRITER_VECTOR_2_H_

#include "Usul/IO/TextReader.h"
#include "Usul/IO/TextWriter.h"
#include "Usul/Math/Vector2.h"

#define USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_2(the_type) \
  USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_2 ( the_type ); \
  USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_2 ( the_type )

USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_2 ( Usul::Math::Vec2c );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_2 ( Usul::Math::Vec2s );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_2 ( Usul::Math::Vec2i );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_2 ( Usul::Math::Vec2l );

USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_2 ( Usul::Math::Vec2uc );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_2 ( Usul::Math::Vec2us );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_2 ( Usul::Math::Vec2ui );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_2 ( Usul::Math::Vec2ul );

USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_2 ( Usul::Math::Vec2f );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_2 ( Usul::Math::Vec2d );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_2 ( Usul::Math::Vec2ld );


#endif // _USUL_IO_TEXT_WRITER_VECTOR_2_H_
