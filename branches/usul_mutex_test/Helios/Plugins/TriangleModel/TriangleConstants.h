
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Constants pertaining to triangle models.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TRIANGLE_MODEL_CONSTANTS_H_
#define _TRIANGLE_MODEL_CONSTANTS_H_

#include "Usul/Types/Types.h"

#include <string>


namespace FileFormat
{
  namespace Record
  {
    // Changing these number will break backward compatability.
    // However, adding new numbers does not break anything.
    typedef Usul::Types::Uint32 Type;
    const Type META_DATA         ( 100 );
    const Type VERTICES          ( 200 );
    const Type TRIANGLE_NORMALS  ( 300 );
    const Type VERTEX_NORMALS    ( 400 );
    const Type TRIANGLES         ( 500 );
    const Type BOUNDING_BOX      ( 600 );
    const Type SORTED_VERTICES   ( 700 );
    const Type VERTEX_LOOP       ( 800 );
    const Type VERTEX_COLORS     ( 900 );
  }

  namespace Header
  {
    // This should never change.
    const char ENDIAN_BIG[]    = { 'B','I','G','E' };
    const char ENDIAN_LITTLE[] = { 'L','I','T','E' };
    const char ID[]            = { '2','3','6','d','5','b','9','1',
                                   '-','d','6','f','d','-','4','3',
                                   '1','a','-','b','2','1','2','-',
                                   'f','3','0','9','f','5','5','3',
                                   '4','1','e','3'};

    // File version.
    typedef Usul::Types::Uint32 VersionNumberType;
    const VersionNumberType VERSION_MAJOR ( 0x00000000u ); 
    const VersionNumberType VERSION_MINOR ( 0x00000001u );
  }

  namespace Size
  {
    typedef Usul::Types::Uint64 Sequence;
    typedef Usul::Types::Uint16 Scalar;
    typedef Usul::Types::Uint64 Record;
  }
}


#endif // _TRIANGLE_MODEL_CONSTANTS_H_
