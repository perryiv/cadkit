
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Classes whose constructors/destructors will set/reset the stream.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_STREAM_SET_RESET_H_
#define _CADKIT_STANDARD_LIBRARY_STREAM_SET_RESET_H_

#include <iostream>


namespace CadKit {


///////////////////////////////////////////////////////////////////////////////
//
//  SlStreamSetReset: Sets/resets the stream flags.
//
///////////////////////////////////////////////////////////////////////////////

struct SlStreamSetReset
{
  SlStreamSetReset ( std::ostream &out ) :
    _out ( out ),
    _flags ( out.flags() )
  {
    // Empty.
  }

  ~SlStreamSetReset()
  {
    // Reset things to where we found them.
    _out.flags ( _flags );
  }

protected:

  std::ostream &_out;
  std::ios_base::fmtflags _flags;
};


///////////////////////////////////////////////////////////////////////////////
//
//  SlOstreamSetReset: Sets/resets the stream flags and number of decimals.
//
///////////////////////////////////////////////////////////////////////////////

struct SlOstreamSetReset : public SlStreamSetReset
{
  SlOstreamSetReset ( std::ostream &out, unsigned int numDecimals ) :
    SlStreamSetReset ( out ),
    _numDecimals ( out.precision ( numDecimals ) )
  {
    // Add the "fixed" flag so that "numDecimals" is the number of decimals.
    _out.setf ( std::ios_base::fixed, std::ios_base::floatfield );
  }

  ~SlOstreamSetReset()
  {
    // Reset things to where we found them.
    _out.precision ( _numDecimals );
  }

protected:

  unsigned int _numDecimals;
};


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_STREAM_SET_RESET_H_
