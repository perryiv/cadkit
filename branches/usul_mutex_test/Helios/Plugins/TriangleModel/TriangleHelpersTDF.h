
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper classes and functions for TDF reading.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TRIANGLE_MODEL_TDF_READER_HELPERS_H_
#define _TRIANGLE_MODEL_TDF_READER_HELPERS_H_

#include "TriangleConstants.h"

#include "Usul/Math/Vector3.h"
#include "Usul/Endian/Endian.h"
#include "Usul/Cast/Cast.h"
#include "Usul/IO/BinaryReader.h"

#include "osg/Vec3f"
#include "osg/Vec4f"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>


namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Functor to swap bytes in containers.
//
///////////////////////////////////////////////////////////////////////////////

struct SwapBytes
{
  void operator () ( osg::Vec3f &v ) const
  {
    Usul::Endian::FromLittleToSystem::convert ( v[0] );
    Usul::Endian::FromLittleToSystem::convert ( v[1] );
    Usul::Endian::FromLittleToSystem::convert ( v[2] );
  }
  void operator () ( osg::Vec4f &v ) const
  {
    Usul::Endian::FromLittleToSystem::convert ( v[0] );
    Usul::Endian::FromLittleToSystem::convert ( v[1] );
    Usul::Endian::FromLittleToSystem::convert ( v[2] );
    Usul::Endian::FromLittleToSystem::convert ( v[3] );
  }
  void operator () ( unsigned int &v ) const
  {
    Usul::Endian::FromLittleToSystem::convert ( v );
  }
  void operator () ( Usul::Math::Vec3ui &v ) const
  {
    Usul::Endian::FromLittleToSystem::convert ( v[0] );
    Usul::Endian::FromLittleToSystem::convert ( v[1] );
    Usul::Endian::FromLittleToSystem::convert ( v[2] );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to check the end of the file.
//
///////////////////////////////////////////////////////////////////////////////

template < class FileInfoType > inline void checkEndOfFile ( std::ifstream &in, FileInfoType &file )
{
  if ( in.eof() )
  {
    Usul::Exceptions::Thrower<std::runtime_error> 
      ( "Error 3344951690: unexpected end of file while reading: ", file.first );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to check if file is big enough.
//
///////////////////////////////////////////////////////////////////////////////

template < class FileInfoType > inline void isFileBigEnough ( std::ifstream &in, FileFormat::Size::Record size, FileInfoType &file )
{
  const Usul::Types::Uint64 position   ( in.tellg() );
  const Usul::Types::Uint64 recordSize ( size );
  const Usul::Types::Uint64 totalSize  ( file.second );

  if ( position + recordSize > totalSize )
  {
    Usul::Exceptions::Thrower<std::runtime_error> 
      ( "Error 3029281709: record size of ", size, " read at position ", position, " exceeds end of file" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to check the size.
//
///////////////////////////////////////////////////////////////////////////////

template < class FileInfoType > 
inline void checkSizes ( std::ifstream &in, const std::string &name, 
                         FileFormat::Size::Record size1, FileFormat::Size::Record size2, 
                         FileInfoType &file )
{
  if ( size1 != size2 )
  {
    const Usul::Types::Uint64 p ( in.tellg() );
    const Usul::Types::Uint64 s ( sizeof ( FileFormat::Size::Scalar ) );
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 3456367539: unsupported ", name, " size ", size2, 
        " at position ", p - s, " in file: ", file.first );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for reading values and reporting errors.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct Read
{
  /////////////////////////////////////////////////////////////////////////////
  //
  // Read one value.
  //
  /////////////////////////////////////////////////////////////////////////////

  static T value ( std::ifstream &in, const std::string &name, bool zeroIsError = true )
  {
    // Read the number.
    T t ( 0 );
    Usul::IO::Binary::ReadLittleEndian::read ( in, t );

    // Make sure it is non-zero.
    if ( zeroIsError && 0 == t )
    {
      const Usul::Types::Uint64 p ( in.tellg() );
      const Usul::Types::Uint64 s ( sizeof ( FileFormat::Size::Record ) );
      Usul::Exceptions::Thrower<std::runtime_error> 
        ( "Error 2096339811: zero value for '", name, "' at position ", p - s );
    }

    // Return the value.
    return t;
  }

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Read a sequence of values.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class SizeType > static void sequence ( std::ifstream &in, const std::string &name, SizeType length, T &v )
  {
    // See if this implementation supports the requested number of vertices.
    const Usul::Types::Uint64 maximum ( v.max_size() );
    if ( length > maximum )
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 3838201917: requested sequence length of ", length, 
          " exceeds maximum size ", maximum );
    }

    // Size the sequence.
    v.resize ( static_cast < typename T::size_type > ( length ) );

    // Get pointer to start of sequence.
    typedef typename T::value_type ValueType;
    ValueType *first ( &(v[0]) );
    ValueType *last  ( first + length );

    // Read sequence in largest possible chunks.
    while ( first < last )
    {
      // Determine how many we can read.
      const std::streamsize size ( static_cast < std::streamsize > ( length * sizeof ( ValueType ) ) );

      // Read the chunk.
      in.read ( ( USUL_UNSAFE_CAST ( char *, first ) ), size );

      // Go to the next chunk.
      first += size;
    }

    // Swap the bytes.
    std::for_each ( v.begin(), v.end(), SwapBytes() );
  }
};


}  // namespace Detail


#endif // _TRIANGLE_MODEL_TDF_READER_HELPERS_H_
