
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlVec2IO: Input/output functions for SlVec2.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_VECTOR_2_IO_FUNCTIONS_H_
#define _CADKIT_STANDARD_LIBRARY_VECTOR_2_IO_FUNCTIONS_H_

#include "SlVec2.h"
#include "SlStreamSetReset.h"
#include "SlManip.h"

#include <iostream>
#include <iomanip>


///////////////////////////////////////////////////////////////////////////////
//
//  Output operator. Keep in global namespace.
//
///////////////////////////////////////////////////////////////////////////////

template < typename T > inline std::ostream &operator << ( std::ostream &out, const CadKit::SlVec2<T> &vec )
{
  out << vec[0] << " " << vec[1];
  return out;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Input operator. Keep in global namespace.
//
///////////////////////////////////////////////////////////////////////////////

template < typename T > inline std::istream &operator >> ( std::istream &in, CadKit::SlVec2<T> &vec )
{
  in >> vec[0] >> vec[1];
  return in;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Useful typedefs.
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
typedef SlManip<SlVec2l> SlVec2lManip;
typedef SlManip<SlVec2i> SlVec2iManip;
typedef SlManip<SlVec2s> SlVec2sManip;

typedef SlFloatManip<SlVec2ld> SlVec2ldManip;
typedef SlFloatManip<SlVec2d>  SlVec2dManip;
typedef SlFloatManip<SlVec2f>  SlVec2fManip;
};


/////////////////////////////////////////////////////////////////////////////
//
//  This will write the vector to the stream with the correct width for
//  each element.
//
//  Usage:
//  out << SlManip ( someVec, width );
//
/////////////////////////////////////////////////////////////////////////////

template < typename T > inline std::ostream &operator << ( std::ostream &out, const CadKit::SlManip < CadKit::SlVec2<T> > &manip )
{
  out << std::setw ( manip.width() ) << manip.value()[0];
  out << std::setw ( manip.width() ) << manip.value()[1];
  return out;
}


/////////////////////////////////////////////////////////////////////////////
//
//  This will write the vector to the stream with the correct number of 
//  decimals and total width of each element.
//
//  Usage:
//  out << SlFloatManip ( someVec, numDecimals, width );
//
/////////////////////////////////////////////////////////////////////////////

template < typename T > inline std::ostream &operator << ( std::ostream &out, const CadKit::SlFloatManip < CadKit::SlVec2<T> > &manip )
{
  CadKit::SlOstreamSetReset reset ( out, manip.numDecimals() );
  out << std::setw ( manip.width() ) << manip.value()[0];
  out << std::setw ( manip.width() ) << manip.value()[1];
  return out;
}


#endif // _CADKIT_STANDARD_LIBRARY_VECTOR_2_IO_FUNCTIONS_H_
