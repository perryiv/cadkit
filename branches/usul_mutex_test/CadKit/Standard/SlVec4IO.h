
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlVec4IO: Input/output functions for SlVec4.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_VECTOR_4_IO_FUNCTIONS_H_
#define _CADKIT_STANDARD_LIBRARY_VECTOR_4_IO_FUNCTIONS_H_

#include "SlVec4.h"
#include "SlStreamSetReset.h"
#include "SlManip.h"

#include <iostream>
#include <iomanip>


///////////////////////////////////////////////////////////////////////////////
//
//  Output operator. Keep in global namespace.
//
///////////////////////////////////////////////////////////////////////////////

template < typename T > inline std::ostream &operator << ( std::ostream &out, const CadKit::SlVec4<T> &vec )
{
  out << vec[0] << " " << vec[1] << " " << vec[2] << " " << vec[3];
  return out;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Input operator. Keep in global namespace.
//
///////////////////////////////////////////////////////////////////////////////

template < typename T > inline std::istream &operator >> ( std::istream &in, CadKit::SlVec4<T> &vec )
{
  in >> vec[0] >> vec[1] >> vec[2] >> vec[3];
  return in;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Useful typedefs.
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
typedef SlManip<SlVec4l> SlVec4lManip;
typedef SlManip<SlVec4i> SlVec4iManip;
typedef SlManip<SlVec4s> SlVec4sManip;

typedef SlFloatManip<SlVec4ld> SlVec4ldManip;
typedef SlFloatManip<SlVec4d>  SlVec4dManip;
typedef SlFloatManip<SlVec4f>  SlVec4fManip;
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

template < typename T > inline std::ostream &operator << ( std::ostream &out, const CadKit::SlManip < CadKit::SlVec4<T> > &manip )
{
  out << std::setw ( manip.width() ) << manip.value()[0];
  out << std::setw ( manip.width() ) << manip.value()[1];
  out << std::setw ( manip.width() ) << manip.value()[2];
  out << std::setw ( manip.width() ) << manip.value()[3];
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

template < typename T > inline std::ostream &operator << ( std::ostream &out, const CadKit::SlFloatManip < CadKit::SlVec4<T> > &manip )
{
  CadKit::SlOstreamSetReset reset ( out, manip.numDecimals() );
  out << std::setw ( manip.width() ) << manip.value()[0];
  out << std::setw ( manip.width() ) << manip.value()[1];
  out << std::setw ( manip.width() ) << manip.value()[2];
  out << std::setw ( manip.width() ) << manip.value()[3];
  return out;
}


#endif // _CADKIT_STANDARD_LIBRARY_VECTOR_4_IO_FUNCTIONS_H_
