
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For reversing bytes and bits.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_LIBRARY_ENDIAN_H_
#define _USUL_LIBRARY_ENDIAN_H_

#include "Usul/Types/Types.h"
#include "Usul/Cast/Cast.h"

///////////////////////////////////////////////////////////////////////////////
//
//  For 1025, which, in binary is:
//  00000000 00000000 00000100 00000001
//  Address  Big-Endian  Little-Endian
//  00       00000000    00000001
//  01       00000000    00000100
//  02       00000100    00000000
//  03       00000001    00000000
//
///////////////////////////////////////////////////////////////////////////////


namespace Usul {
namespace Endian {


///////////////////////////////////////////////////////////////////////////////
//
//  See if the machine is big endian.
//
///////////////////////////////////////////////////////////////////////////////

inline bool isBig()
{
  unsigned long u = 1;
  return ( 0 == ( *( (char *) &u ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the machine is little endian.
//
///////////////////////////////////////////////////////////////////////////////

inline bool isLittle()
{
  return ( false == Usul::Endian::isBig() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Details.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Reverse the bytes of the integer.
//
///////////////////////////////////////////////////////////////////////////////

inline void _reverseBytes ( Usul::Types::Uint16 &n )
{
  unsigned char *cptr = (unsigned char *) &n;
  unsigned char tmp = cptr[0];
  cptr[0] = cptr[1];
  cptr[1] = tmp;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reverse the bytes of the integer.
//
//////////////////////////////////////////////////////////////////////////

inline void _reverseBytes ( Usul::Types::Uint32 &n )
{
  unsigned char *cptr = (unsigned char *) &n;
  unsigned char tmp = cptr[0];
  cptr[0] = cptr[3];
  cptr[3] = tmp;
  tmp     = cptr[1];
  cptr[1] = cptr[2];
  cptr[2] = tmp;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reverse the bytes of the integer.
//
///////////////////////////////////////////////////////////////////////////////

inline void _reverseBytes ( Usul::Types::Uint64 &n )
{
  unsigned char *cptr = (unsigned char *) &n;
  unsigned char tmp = cptr[0];
  cptr[0] = cptr[7];
  cptr[7] = tmp;
  tmp     = cptr[1];
  cptr[1] = cptr[6];
  cptr[6] = tmp;
  tmp     = cptr[2];
  cptr[2] = cptr[5];
  cptr[5] =tmp;
  tmp     = cptr[3];
  cptr[3] = cptr[4];
  cptr[4] = tmp;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Template definition and specialization for reversing the bytes.
//
///////////////////////////////////////////////////////////////////////////////

template < unsigned int Size > struct ReverseBytes;
template <> struct ReverseBytes < sizeof ( Usul::Types::Uint16 ) >
{
  void operator () ( Usul::Types::Uint16 &n ) const
  {
    Usul::Endian::Detail::_reverseBytes ( n );
  }
  void operator () ( Usul::Types::Int16 &n ) const
  {
    Usul::Endian::Detail::_reverseBytes (  USUL_UNSAFE_CAST ( Usul::Types::Uint16&, n ) );
  }
};
template <> struct ReverseBytes < sizeof ( Usul::Types::Uint32 ) >
{
  void operator () ( Usul::Types::Uint32 &n ) const
  {
    Usul::Endian::Detail::_reverseBytes ( n );
  }
  void operator () ( Usul::Types::Int32 &n ) const
  {
    Usul::Endian::Detail::_reverseBytes ( USUL_UNSAFE_CAST ( Usul::Types::Uint32&, n ) );
  }
};
template <> struct ReverseBytes < sizeof ( Usul::Types::Uint64 ) >
{
  void operator () ( Usul::Types::Uint64 &n ) const
  {
    Usul::Endian::Detail::_reverseBytes ( n );
  }
  void operator () ( Usul::Types::Int64 &n ) const
  {
    Usul::Endian::Detail::_reverseBytes ( USUL_UNSAFE_CAST ( Usul::Types::Uint64&, n ) );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  End of details.
//
///////////////////////////////////////////////////////////////////////////////

};


///////////////////////////////////////////////////////////////////////////////
//
//  Reverse the bytes of the data.
//
///////////////////////////////////////////////////////////////////////////////

template < class Type > inline void reverseBytes ( Type &n )
{
  typedef Usul::Endian::Detail::ReverseBytes < sizeof ( Type ) > ReverseBytes;
  ReverseBytes () ( n );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reverse all the bits in a 32 bit unsigned integer. This is normally 
//  not used in the big-endian-little-endian swap. That usually just 
//  deals with whole bytes. 
//  Snagged from: http://www.sjbaker.org/steve/software/cute_code.html
//
///////////////////////////////////////////////////////////////////////////////

inline void reverseBits ( Usul::Types::Uint32 &n )
{
  n = ((n >>  1) & 0x55555555) | ((n <<  1) & 0xaaaaaaaa);
  n = ((n >>  2) & 0x33333333) | ((n <<  2) & 0xcccccccc);
  n = ((n >>  4) & 0x0f0f0f0f) | ((n <<  4) & 0xf0f0f0f0);
  n = ((n >>  8) & 0x00ff00ff) | ((n <<  8) & 0xff00ff00);
  n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reverse all the bits in a 32 bit signed integer. This is normally 
//  not used in the big-endian-little-endian swap. That usually just 
//  deals with whole bytes.
//  Snagged from: http://www.sjbaker.org/steve/software/cute_code.html
//
///////////////////////////////////////////////////////////////////////////////

inline void reverseBits ( Usul::Types::Int32 &n )
{
  n = ((n >>  1) & 0x55555555) | ((n <<  1) & 0xaaaaaaaa);
  n = ((n >>  2) & 0x33333333) | ((n <<  2) & 0xcccccccc);
  n = ((n >>  4) & 0x0f0f0f0f) | ((n <<  4) & 0xf0f0f0f0);
  n = ((n >>  8) & 0x00ff00ff) | ((n <<  8) & 0xff00ff00);
  n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000);
}


}; // namespace Endian
}; // namespace Usul


#endif // _USUL_LIBRARY_ENDIAN_H_
