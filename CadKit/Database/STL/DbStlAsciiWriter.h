
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbStlAsciiWriter: Small class to print ascii.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_ASCII_WRITER_H_
#define _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_ASCII_WRITER_H_

#include "Database/Base/DbBaseAsciiWriter.h"


namespace CadKit
{
class DbStlAsciiWriter : public DbBaseAsciiWriter
{
public:

  DbStlAsciiWriter ( std::ostream &out, unsigned int numDecimals, unsigned int width ) : 
    DbBaseAsciiWriter ( out, numDecimals, width )
  {
    // Empty.
  }

  void operator() ( const DbStlTriangle &facet )
  {
    _out << "facet normal " << this->_write ( facet.getNormal() ) << '\n';
    _out << "outer loop\n";
    _out << "vertex " << this->_write ( facet.getVertex ( 0 ) ) << '\n';
    _out << "vertex " << this->_write ( facet.getVertex ( 1 ) ) << '\n';
    _out << "vertex " << this->_write ( facet.getVertex ( 2 ) ) << '\n';
    _out << "endloop\n";
    _out << "endfacet\n";
  }
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_ASCII_WRITER_H_
