
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbStlBinaryWriter: Small class to print binary.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_BINARY_WRITER_H_
#define _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_BINARY_WRITER_H_

#include "Standard/SlEndian.h"

#include <iostream>


namespace CadKit
{
class DbStlBinaryWriter
{
public:

  typedef SlVec3<SlFloat32> Vec;

  DbStlBinaryWriter ( std::ostream &out ) : _out ( out ), _reverse ( CadKit::isBigEndian() ){}

  void write ( SlUint32 data ) // Take a copy.
  {
    this->_endian ( data );
    _out.write ( reinterpret_cast<char *> ( &data ), sizeof ( SlUint32 ) );
  }

  void operator() ( const DbStlTriangle &facet )
  {
    // Write the normal and vertices.
    this->_write ( facet.getNormal() );
    this->_write ( facet.getVertex ( 0 ) );
    this->_write ( facet.getVertex ( 1 ) );
    this->_write ( facet.getVertex ( 2 ) );

    // Write the 2-byte buffer.
    const char buffer[2] = { '\0', '\0' };
    _out.write ( buffer, 2 );
  }

private:

  std::ostream &_out;
  bool _reverse;

  void _endian ( SlUint32 &data )
  {
    if ( _reverse )
      CadKit::reverseBytes ( data );
  }

  void _endian ( Vec &v )
  {
    if ( _reverse )
    {
      CadKit::reverseBytes ( v[0] );
      CadKit::reverseBytes ( v[1] );
      CadKit::reverseBytes ( v[2] );
    }
  }

  void _write ( Vec v ) // Take a copy because of byte swap.
  {
    this->_endian ( v );
    _out.write ( reinterpret_cast<char *> ( &(v[0]) ), sizeof ( SlFloat32 ) );
    _out.write ( reinterpret_cast<char *> ( &(v[1]) ), sizeof ( SlFloat32 ) );
    _out.write ( reinterpret_cast<char *> ( &(v[2]) ), sizeof ( SlFloat32 ) );
  }
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_BINARY_WRITER_H_
