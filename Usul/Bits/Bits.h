
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


}; // namespace Bits
}; // namespace Usul


#endif // _USUL_BITS_H_
