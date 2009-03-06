
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
//  Conversions for OGC's well-known-binary format.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_CONVERT_WELL_KNOWN_BINARY_H_
#define _USUL_CONVERT_WELL_KNOWN_BINARY_H_

#include "Usul/Convert/Generic.h"
#include "Usul/Cast/Cast.h"
#include "Usul/Endian/Endian.h"
#include "Usul/Math/Vector2.h"
#include "Usul/MPL/SameType.h"
#include "Usul/Strings/Format.h"

#include <stdexcept>
#include <vector>


///////////////////////////////////////////////////////////////////////////////
//
//  Conversion for point.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{ 
  namespace Convert
  {
    template <> struct Type < std::vector<unsigned char>, Usul::Math::Vec2d >
    {
      typedef std::vector<unsigned char> Blob;
      typedef Type < Blob, Usul::Math::Vec2d > ThisType;

      enum
      {
        NUM_BYTES = ( sizeof ( unsigned char ) + sizeof ( Usul::Types::Uint32 ) + 2 * sizeof ( Usul::Types::Float64 ) ),
        POINT_TYPE = 1
      };

      static void convert ( const Blob &from, Usul::Math::Vec2d &to )
      {
        USUL_ASSERT_SAME_TYPE ( Usul::Types::Float64, Usul::Math::Vec2d::value_type );

        if ( ThisType::NUM_BYTES != from.size() )
        {
          throw std::invalid_argument ( Usul::Strings::format 
            ( "Error 3366212750: Invalid character vector size ", from.size(), ", expected ", ThisType::NUM_BYTES ) );
        }

        const unsigned char *endianPtr ( &from[0] );
        const unsigned char endian ( *endianPtr ); ++endianPtr;
        const Usul::Types::Uint32 *typePtr ( USUL_UNSAFE_CAST ( Usul::Types::Uint32 *, endianPtr ) );
        Usul::Types::Uint32 type ( *typePtr ); ++typePtr;
        const Usul::Types::Float64 *values ( USUL_UNSAFE_CAST ( Usul::Types::Float64 * , typePtr ) );
        Usul::Types::Float64 x ( values[0] );
        Usul::Types::Float64 y ( values[1] );

        if ( 0 == endian )
        {
          Usul::Endian::FromBigToSystem::convert ( type );
          Usul::Endian::FromBigToSystem::convert ( x );
          Usul::Endian::FromBigToSystem::convert ( y );
        }
        else
        {
          Usul::Endian::FromLittleToSystem::convert ( type );
          Usul::Endian::FromLittleToSystem::convert ( x );
          Usul::Endian::FromLittleToSystem::convert ( y );
        }

        if ( ThisType::POINT_TYPE != type )
        {
          throw std::invalid_argument ( Usul::Strings::format
            ( "Error 2344339624: Invalid geometry type ", type, ", expected ", ThisType::POINT_TYPE ) );
        }

        to[0] = x;
        to[1] = y;
      }

      static Usul::Math::Vec2d convert ( const Blob &from )
      {
        Usul::Math::Vec2d to;
        ThisType::convert ( from, to );
        return to;
      }
    };
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Conversion for line. See:
//  http://www.sqlskills.com/blogs/simon/post/SQL-Server-2008-Spatial-Using-it-in-Net-code.aspx
//  http://edndoc.esri.com/arcsde/9.0/general_topics/wkb_representation.htm
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{ 
  namespace Convert
  {
    template <> struct Type < std::vector<unsigned char>, std::vector<Usul::Math::Vec2d> >
    {
      typedef std::vector<unsigned char> Blob;
      typedef std::vector<Usul::Math::Vec2d> Line;
      typedef Type < Blob, Line > ThisType;

      enum
      {
        MIN_BYTES = ( sizeof ( unsigned char ) + 2 * sizeof ( Usul::Types::Uint32 ) ),
        LINE_TYPE = 2
      };

      static void convert ( const Blob &from, Line &to )
      {
        USUL_ASSERT_SAME_TYPE ( Usul::Types::Float64, Usul::Math::Vec2d::value_type );

        // Initialize.
        to.clear();

        if ( ThisType::MIN_BYTES > from.size() )
        {
          throw std::invalid_argument ( Usul::Strings::format 
            ( "Error 3438257445: Invalid character vector size ", from.size(), ", expected at least ", ThisType::MIN_BYTES ) );
        }

        const unsigned char *endianPtr ( &from[0] );
        const unsigned char endian ( *endianPtr ); ++endianPtr;
        const Usul::Types::Uint32 *uIntPtr ( USUL_UNSAFE_CAST ( Usul::Types::Uint32 *, endianPtr ) );
        Usul::Types::Uint32 type ( *uIntPtr ); ++uIntPtr;
        Usul::Types::Uint32 count ( *uIntPtr ); ++uIntPtr;
        const Usul::Types::Float64 *values ( USUL_UNSAFE_CAST ( Usul::Types::Float64 * , uIntPtr ) );

        if ( 0 == endian )
        {
          Usul::Endian::FromBigToSystem::convert ( type );
          Usul::Endian::FromBigToSystem::convert ( count );
        }

        else
        {
          Usul::Endian::FromLittleToSystem::convert ( type );
          Usul::Endian::FromLittleToSystem::convert ( count );
        }

        if ( ThisType::LINE_TYPE != type )
        {
          throw std::invalid_argument ( Usul::Strings::format
            ( "Error 1391184162: Invalid geometry type ", type, ", expected ", ThisType::LINE_TYPE ) );
        }

        // If the line is empty then we're done.
        if ( 0 == count )
        {
          return;
        }

        // Make space in the vector.
        to.reserve ( count );

        // Loop through the points.
        for ( Usul::Types::Uint32 i = 0; i < count; ++i )
        {
          Usul::Types::Float64 x ( *values ); ++values;
          Usul::Types::Float64 y ( *values ); ++values;

          if ( 0 == endian )
          {
            Usul::Endian::FromBigToSystem::convert ( x );
            Usul::Endian::FromBigToSystem::convert ( y );
          }

          else
          {
            Usul::Endian::FromLittleToSystem::convert ( x );
            Usul::Endian::FromLittleToSystem::convert ( y );
          }

          to.push_back ( Usul::Math::Vec2d ( x, y ) );
        }
      }

      static Line convert ( const Blob &from )
      {
        Line to;
        ThisType::convert ( from, to );
        return to;
      }
    };
  }
}


#endif // _USUL_CONVERT_WELL_KNOWN_BINARY_H_
