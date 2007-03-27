
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Binary read convenience functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_IO_BINARY_READER_H_
#define _USUL_IO_BINARY_READER_H_

#include "Usul/Endian/Endian.h"
#include "Usul/Cast/Cast.h"

#include <stdexcept>


namespace Usul {
namespace IO {


///////////////////////////////////////////////////////////////////////////////
//
//  For reading binary data and converting to the system's byte order.
//
///////////////////////////////////////////////////////////////////////////////

template < class EndianPolicy_ > struct Reader
{
  typedef EndianPolicy_ EndianPolicy;

  template < class Stream, class T > static void read ( Stream &stream, T &t )
  {
    if ( EOF == stream.peek() )
      throw std::runtime_error ( "Error 1496689971: Unexpected end of file reached" );
    stream.read ( USUL_UNSAFE_CAST ( char *, &t ), sizeof ( T ) );
    EndianPolicy::convert ( t );
  }
  template < class Stream, class T1, class T2 > static void read ( Stream &stream, T1 &t1, T2 &t2 )
  {
    read ( stream, t1 );
    read ( stream, t2 );
  }
  template < class Stream, class T1, class T2, class T3 > static void read ( Stream &stream, T1 &t1, T2 &t2, T3 &t3 )
  {
    read ( stream, t1 );
    read ( stream, t2 );
    read ( stream, t3 );
  }
  template < class Stream, class T1, class T2, class T3, class T4 > static void read ( Stream &stream, T1 &t1, T2 &t2, T3 &t3, T4 &t4 )
  {
    read ( stream, t1 );
    read ( stream, t2 );
    read ( stream, t3 );
    read ( stream, t4 );
  }
  template < class Stream, class T > static void read2 ( Stream &stream, T &t )
  {
    Usul::IO::Reader<EndianPolicy>::read ( stream, t[0], t[1] );
  }
  template < class Stream, class T > static void read3 ( Stream &stream, T &t )
  {
    Usul::IO::Reader<EndianPolicy>::read ( stream, t[0], t[1], t[2] );
  }
  template < class Stream, class T > static void read4 ( Stream &stream, T &t )
  {
    Usul::IO::Reader<EndianPolicy>::read ( stream, t[0], t[1], t[2], t[3] );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Convenient typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Reader < Usul::Endian::FromBigToSystem >    ReadBigEndian;
typedef Reader < Usul::Endian::FromLittleToSystem > ReadLittleEndian;


}; // namespace IO
}; // namespace Usul


#endif // _USUL_IO_BINARY_READER_H_
