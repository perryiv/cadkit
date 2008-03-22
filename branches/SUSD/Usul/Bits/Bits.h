
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For working with bits.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_BITS_H_
#define _USUL_BITS_H_

#include "Usul/MPL/StaticAssert.h"

namespace Usul {
namespace Bits {


//////////////////////////////////////////////////////////////////////////
//
//  See if the "bits" are in "number".
//
//////////////////////////////////////////////////////////////////////////

template < class N, class B > inline bool has ( N number, B bits )
{
  return ( ( number & bits ) == ( static_cast<N>(bits) ) );
}


//////////////////////////////////////////////////////////////////////////
//
//  Add the "bits" to "number".
//
//////////////////////////////////////////////////////////////////////////

template < class N, class B > inline N add ( N number, B bits )
{
  return ( number | bits );
}


//////////////////////////////////////////////////////////////////////////
//
//  Remove the "bits" from "number".
//
//////////////////////////////////////////////////////////////////////////

template < class N, class B > inline N remove ( N number, B bits )
{
  return ( ( number & bits ) ? ( number ^ bits ) : number );
}


//////////////////////////////////////////////////////////////////////////
//
//  Toggle the "bits" in "number".
//
//////////////////////////////////////////////////////////////////////////

template < class N, class B > inline N toggle ( N number, B bits )
{
  return ( number ^ bits );
}


//////////////////////////////////////////////////////////////////////////
//
//  Set the "bits" in "number".
//
//////////////////////////////////////////////////////////////////////////

template < class N, class B > inline N set ( N number, B bits, bool state )
{
  return ( ( state ) ? Usul::Bits::add ( number, bits ) : Usul::Bits::remove ( number, bits ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reverse all the bits in a 32 bit value.
//  Snagged from: http://www.sjbaker.org/steve/software/cute_code.html
//
///////////////////////////////////////////////////////////////////////////////

template < class N > inline void reverse ( N &n )
{
  USUL_STATIC_ASSERT ( 32 == sizeof ( N ) );

  n = ((n >>  1) & 0x55555555) | ((n <<  1) & 0xaaaaaaaa);
  n = ((n >>  2) & 0x33333333) | ((n <<  2) & 0xcccccccc);
  n = ((n >>  4) & 0x0f0f0f0f) | ((n <<  4) & 0xf0f0f0f0);
  n = ((n >>  8) & 0x00ff00ff) | ((n <<  8) & 0xff00ff00);
  n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000);
}


}; // namespace Bits
}; // namespace Usul


#endif // _USUL_BITS_H_