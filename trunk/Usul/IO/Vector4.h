
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

#ifndef _USUL_IO_TEXT_WRITER_VECTOR_4_H_
#define _USUL_IO_TEXT_WRITER_VECTOR_4_H_

#include "Usul/IO/TextReader.h"
#include "Usul/IO/TextWriter.h"
#include "Usul/Math/Vector4.h"


#define USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_4(the_type) \
  USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_4 ( the_type ); \
  USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_4 ( the_type )

USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_4 ( Usul::Math::Vec4c );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_4 ( Usul::Math::Vec4s );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_4 ( Usul::Math::Vec4i );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_4 ( Usul::Math::Vec4l );

USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_4 ( Usul::Math::Vec4uc );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_4 ( Usul::Math::Vec4us );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_4 ( Usul::Math::Vec4ui );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_4 ( Usul::Math::Vec4ul );

USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_4 ( Usul::Math::Vec4f );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_4 ( Usul::Math::Vec4d );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_4 ( Usul::Math::Vec4ld );


#endif // _USUL_IO_TEXT_WRITER_VECTOR_4_H_
