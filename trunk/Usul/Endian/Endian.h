
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
//  Compile-time detection of byte order. See boost/detail/limits.hpp and 
//  http://www.unixpapa.com/incnote/byteorder.html
//
///////////////////////////////////////////////////////////////////////////////

#if   defined ( __sparc     ) || \
      defined ( __sparc__   ) || \
      defined ( __powerpc__ ) || \
      defined ( __ppc__     ) || \
      defined ( __hppa      ) || \
      defined ( _MIPSEB     ) || \
      defined ( _POWER      )
#define USUL_BIG_ENDIAN
#elif defined ( __i386__    ) || \
      defined ( __alpha__   ) || \
      defined ( __ia64      ) || \
      defined ( __ia64__    ) || \
      defined ( i386        ) || \
      defined ( __i386__    ) || \
      defined ( _M_IX86     ) || \
      defined ( vax         ) || \
      defined ( __alpha     )
#define USUL_LITTLE_ENDIAN
#else
#error Unknown endian type in Usul/Endian/Endian.h
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Run-time checking for big endian.
//
///////////////////////////////////////////////////////////////////////////////

inline bool isBig()
{
  unsigned long u = 1;
  return ( 0 == ( *( (char *) &u ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run-time checking for little endian.
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
  void operator () ( Usul::Types::Float32 &n ) const
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
  void operator () ( Usul::Types::Float64 &n ) const
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
//  Convert the bytes.
//
///////////////////////////////////////////////////////////////////////////////

struct FromSystemToBig
{
  template < class T > static void convert ( T &t )
  {
    #ifdef USUL_LITTLE_ENDIAN
    Usul::Endian::reverseBytes ( t );
    #endif
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the bytes.
//
///////////////////////////////////////////////////////////////////////////////

struct FromSystemToLittle
{
  template < class T > static void convert ( T &t )
  {
    #ifdef USUL_BIG_ENDIAN
    Usul::Endian::reverseBytes ( t );
    #endif
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the bytes.
//
///////////////////////////////////////////////////////////////////////////////

struct FromBigToSystem
{
  template < class T > static void convert ( T &t )
  {
    #ifdef USUL_LITTLE_ENDIAN
    Usul::Endian::reverseBytes ( t );
    #endif
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the bytes.
//
///////////////////////////////////////////////////////////////////////////////

struct FromLittleToSystem
{
  template < class T > static void convert ( T &t )
  {
    #ifdef USUL_BIG_ENDIAN
    Usul::Endian::reverseBytes ( t );
    #endif
  }
};


}; // namespace Endian
}; // namespace Usul


#endif // _USUL_LIBRARY_ENDIAN_H_
