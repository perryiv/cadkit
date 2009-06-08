
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbBaseAsciiWriter: Shared functionality for functors that print ascii.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_BASE_LIBRARY_ASCII_WRITER_H_
#define _CADKIT_DATABASE_BASE_LIBRARY_ASCII_WRITER_H_

#include "Standard/SlVec3IO.h"
#include "Standard/SlAssert.h"
#include "Standard/SlManip.h"


namespace CadKit {


class DbBaseAsciiWriter
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  DbBaseAsciiWriter ( std::ostream &out, unsigned int numDecimals, unsigned int width ) : 
    _out ( out ),
    _numDecimals ( numDecimals ),
    _width ( width )
  {
    SL_ASSERT ( width > numDecimals );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ~DbBaseAsciiWriter()
  {
    // There is nothing to delete. When "_reset" does out of scope it will 
    // reset the stream "_out" to where it was when this instance was created.
  }

protected:

  std::ostream &_out;
  unsigned int _numDecimals;
  unsigned int _width;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return a manipulator.
  //
  /////////////////////////////////////////////////////////////////////////////

  SlFloatManip<SlVec3f> _write ( const SlVec3f &vec )
  {
    return SlFloatManip<SlVec3f> ( vec, _numDecimals, _width );
  }
};


}; // namespace CadKit


#endif // _CADKIT_DATABASE_BASE_LIBRARY_ASCII_WRITER_H_
