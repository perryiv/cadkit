
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlVec3IO: Input/output functions for SlVec3.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_VECTOR_3_IO_FUNCTIONS_H_
#define _CADKIT_STANDARD_LIBRARY_VECTOR_3_IO_FUNCTIONS_H_

#include "SlVec3.h"
#include "SlStreamSetReset.h"
#include "SlManip.h"

#include <iostream>
#include <iomanip>


///////////////////////////////////////////////////////////////////////////////
//
//  Output operator. Keep in global namespace.
//
///////////////////////////////////////////////////////////////////////////////

template < typename T > inline std::ostream &operator << ( std::ostream &out, const CadKit::SlVec3<T> &vec )
{
  out << vec[0] << " " << vec[1] << " " << vec[2];
  return out;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Input operator. Keep in global namespace.
//
///////////////////////////////////////////////////////////////////////////////

template < typename T > inline std::istream &operator >> ( std::istream &in, CadKit::SlVec3<T> &vec )
{
  in >> vec[0] >> vec[1] >> vec[2];
  return in;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Useful typedefs.
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
typedef SlManip<SlVec3l> SlVec3lManip;
typedef SlManip<SlVec3i> SlVec3iManip;
typedef SlManip<SlVec3s> SlVec3sManip;

typedef SlFloatManip<SlVec3ld> SlVec3ldManip;
typedef SlFloatManip<SlVec3d>  SlVec3dManip;
typedef SlFloatManip<SlVec3f>  SlVec3fManip;
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

template < typename T > inline std::ostream &operator << ( std::ostream &out, const CadKit::SlManip < CadKit::SlVec3<T> > &manip )
{
  out << std::setw ( manip.width() ) << manip.value()[0];
  out << std::setw ( manip.width() ) << manip.value()[1];
  out << std::setw ( manip.width() ) << manip.value()[2];
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

template < typename T > inline std::ostream &operator << ( std::ostream &out, const CadKit::SlFloatManip < CadKit::SlVec3<T> > &manip )
{
  CadKit::SlOstreamSetReset reset ( out, manip.numDecimals() );
  out << std::setw ( manip.width() ) << manip.value()[0];
  out << std::setw ( manip.width() ) << manip.value()[1];
  out << std::setw ( manip.width() ) << manip.value()[2];
  return out;
}


#endif // _CADKIT_STANDARD_LIBRARY_VECTOR_3_IO_FUNCTIONS_H_
