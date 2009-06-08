
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

#ifndef _USUL_IO_TEXT_WRITER_VECTOR_3_H_
#define _USUL_IO_TEXT_WRITER_VECTOR_3_H_

#include "Usul/IO/TextReader.h"
#include "Usul/IO/TextWriter.h"
#include "Usul/Math/Vector3.h"


#define USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_3(the_type) \
  USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_3 ( the_type ); \
  USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_3 ( the_type )

USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_3 ( Usul::Math::Vec3c );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_3 ( Usul::Math::Vec3s );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_3 ( Usul::Math::Vec3i );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_3 ( Usul::Math::Vec3l );

USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_3 ( Usul::Math::Vec3uc );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_3 ( Usul::Math::Vec3us );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_3 ( Usul::Math::Vec3ui );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_3 ( Usul::Math::Vec3ul );

USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_3 ( Usul::Math::Vec3f );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_3 ( Usul::Math::Vec3d );
USUL_IO_TEXT_DEFINE_IO_TYPES_VECTOR_3 ( Usul::Math::Vec3ld );


#endif // _USUL_IO_TEXT_WRITER_VECTOR_3_H_
