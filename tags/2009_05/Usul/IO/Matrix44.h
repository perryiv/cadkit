
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
//  Text output for matrix.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_IO_TEXT_WRITER_MATRIX_4_4_H_
#define _USUL_IO_TEXT_WRITER_MATRIX_4_4_H_

#include "Usul/IO/TextReader.h"
#include "Usul/IO/TextWriter.h"
#include "Usul/Math/Matrix44.h"

#define USUL_IO_TEXT_DEFINE_IO_TYPES_MATRIX_4_4(the_type) \
  USUL_IO_TEXT_DEFINE_READER_TYPE_MATRIX_4_4 ( the_type ); \
  USUL_IO_TEXT_DEFINE_WRITER_TYPE_MATRIX_4_4 ( the_type )

USUL_IO_TEXT_DEFINE_IO_TYPES_MATRIX_4_4 ( Usul::Math::Matrix44c );
USUL_IO_TEXT_DEFINE_IO_TYPES_MATRIX_4_4 ( Usul::Math::Matrix44s );
USUL_IO_TEXT_DEFINE_IO_TYPES_MATRIX_4_4 ( Usul::Math::Matrix44i );
USUL_IO_TEXT_DEFINE_IO_TYPES_MATRIX_4_4 ( Usul::Math::Matrix44l );

USUL_IO_TEXT_DEFINE_IO_TYPES_MATRIX_4_4 ( Usul::Math::Matrix44uc );
USUL_IO_TEXT_DEFINE_IO_TYPES_MATRIX_4_4 ( Usul::Math::Matrix44us );
USUL_IO_TEXT_DEFINE_IO_TYPES_MATRIX_4_4 ( Usul::Math::Matrix44ui );
USUL_IO_TEXT_DEFINE_IO_TYPES_MATRIX_4_4 ( Usul::Math::Matrix44ul );

USUL_IO_TEXT_DEFINE_IO_TYPES_MATRIX_4_4 ( Usul::Math::Matrix44f );
USUL_IO_TEXT_DEFINE_IO_TYPES_MATRIX_4_4 ( Usul::Math::Matrix44d );
USUL_IO_TEXT_DEFINE_IO_TYPES_MATRIX_4_4 ( Usul::Math::Matrix44ld );


#endif // _USUL_IO_TEXT_WRITER_MATRIX_4_4_H_
