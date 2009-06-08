
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbItriAsciiWriter: Small class to print ascii.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_ITRI_LIBRARY_ASCII_WRITER_H_
#define _CADKIT_DATABASE_ITRI_LIBRARY_ASCII_WRITER_H_

#include "Database/Base/DbBaseAsciiWriter.h"


namespace CadKit
{
class DbItriAsciiWriter : public DbBaseAsciiWriter
{
public:

  DbItriAsciiWriter ( std::ostream &out, unsigned int numDecimals, unsigned int width ) : 
    DbBaseAsciiWriter ( out, numDecimals, width )
  {
    // Empty.
  }

  void operator() ( const SlVec3<SlVec3f> &triangle )
  {
    _out << this->_write ( triangle[0] ) << '\n';
    _out << this->_write ( triangle[1] ) << '\n';
    _out << this->_write ( triangle[2] ) << '\n';
  }
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_ITRI_LIBRARY_ASCII_WRITER_H_
