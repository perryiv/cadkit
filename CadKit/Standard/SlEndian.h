
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  SlByteReverse: For reversing bytes and bits.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_ENDIAN_H_
#define _CADKIT_STANDARD_LIBRARY_ENDIAN_H_

#include "SlTypedefs.h"


//////////////////////////////////////////////////////////////////////////
//
//  For 1025, which, in binary is:
//  00000000 00000000 00000100 00000001
//  Address  Big-Endian  Little-Endian
//  00       00000000    00000001
//  01       00000000    00000100
//  02       00000100    00000000
//  03       00000001    00000000
//
//////////////////////////////////////////////////////////////////////////


namespace CadKit
{
//////////////////////////////////////////////////////////////////////////
//
//  See if the machine is big endian.
//
//////////////////////////////////////////////////////////////////////////

inline bool isBigEndian()
{
  unsigned long u = 1;
  return ( 0 == ( *( (char *) &u ) ) );
}


//////////////////////////////////////////////////////////////////////////
//
//  See if the machine is little endian.
//
//////////////////////////////////////////////////////////////////////////

inline bool isLittleEndian()
{
  return ( false == isBigEndian() );
}


//////////////////////////////////////////////////////////////////////////
//
//  Reverse the bytes of the data.
//
//////////////////////////////////////////////////////////////////////////

template <class Type> inline bool reverseBytes ( Type &n )
{
  const unsigned int numBytes = sizeof ( Type );

  switch ( numBytes )
  {
  case sizeof ( SlUint16 ):

    reverseBytes16 ( (SlUint16 &) n );
    return true;

  case sizeof ( SlUint32 ):

    reverseBytes32 ( (SlUint32 &) n );
    return true;

  case sizeof ( SlUint64 ):

    reverseBytes64 ( (SlUint64 &) n );
    return true;

  default:

    return false;
  }
}


//////////////////////////////////////////////////////////////////////////
//
//  Reverse the bytes of the integer.
//
//////////////////////////////////////////////////////////////////////////

inline void reverseBytes16 ( SlUint16 &n )
{
  unsigned char *cptr = (unsigned char *) &n;
  unsigned char tmp = cptr[0];
  cptr[0] = cptr[1];
  cptr[1] = tmp;
}


//////////////////////////////////////////////////////////////////////////
//
//  Reverse the bytes of the integer.
//
//////////////////////////////////////////////////////////////////////////

inline void reverseBytes32 ( SlUint32 &n )
{
  unsigned char *cptr = (unsigned char *) &n;
  unsigned char tmp = cptr[0];
  cptr[0] = cptr[3];
  cptr[3] = tmp;
  tmp     = cptr[1];
  cptr[1] = cptr[2];
  cptr[2] = tmp;
}


//////////////////////////////////////////////////////////////////////////
//
//  Reverse the bytes of the integer.
//
//////////////////////////////////////////////////////////////////////////

inline void reverseBytes64 ( SlUint64 &n )
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


//////////////////////////////////////////////////////////////////////////
//
//  Reverse all the bits in a 32 bit unsigned integer. This is normally 
//  not used in the big-endian-little-endian swap. That usually just 
//  deals with whole bytes. 
//  Snagged from: http://www.sjbaker.org/steve/software/cute_code.html
//
//////////////////////////////////////////////////////////////////////////

inline void reverseBits32 ( SlUint32 &n )
{
  n = ((n >>  1) & 0x55555555) | ((n <<  1) & 0xaaaaaaaa);
  n = ((n >>  2) & 0x33333333) | ((n <<  2) & 0xcccccccc);
  n = ((n >>  4) & 0x0f0f0f0f) | ((n <<  4) & 0xf0f0f0f0);
  n = ((n >>  8) & 0x00ff00ff) | ((n <<  8) & 0xff00ff00);
  n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000);
}


//////////////////////////////////////////////////////////////////////////
//
//  Reverse all the bits in a 32 bit signed integer. This is normally 
//  not used in the big-endian-little-endian swap. That usually just 
//  deals with whole bytes.
//  Snagged from: http://www.sjbaker.org/steve/software/cute_code.html
//
//////////////////////////////////////////////////////////////////////////

inline void reverseBits32 ( SlInt32 &n )
{
  n = ((n >>  1) & 0x55555555) | ((n <<  1) & 0xaaaaaaaa);
  n = ((n >>  2) & 0x33333333) | ((n <<  2) & 0xcccccccc);
  n = ((n >>  4) & 0x0f0f0f0f) | ((n <<  4) & 0xf0f0f0f0);
  n = ((n >>  8) & 0x00ff00ff) | ((n <<  8) & 0xff00ff00);
  n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000);
}


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_ENDIAN_H_
