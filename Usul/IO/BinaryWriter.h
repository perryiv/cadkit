
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Binary write convenience functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_IO_BINARY_WRITER_H_
#define _USUL_IO_BINARY_WRITER_H_

#include "Usul/Endian/Endian.h"
#include "Usul/Cast/Cast.h"
#include "Usul/MPL/SameType.h"


namespace Usul {
namespace IO {
namespace Binary {


///////////////////////////////////////////////////////////////////////////////
//
//  For writing binary data and converting to the system's byte order.
//
///////////////////////////////////////////////////////////////////////////////

template < class EndianPolicy_ > struct Writer
{
  typedef EndianPolicy_ EndianPolicy;

  template < class Stream, class T > static void write ( Stream &stream, const T &t )
  {
    T temp ( t );
    EndianPolicy::convert ( temp );
    stream.write ( USUL_UNSAFE_CAST ( char *, &temp ), sizeof ( T ) );
  }
  template < class Stream, class T1, class T2 > static void write ( Stream &stream, const T1 &t1, const T2 &t2 )
  {
    write ( stream, t1 );
    write ( stream, t2 );
  }
  template < class Stream, class T1, class T2, class T3 > static void write ( Stream &stream, const T1 &t1, const T2 &t2, const T3 &t3 )
  {
    write ( stream, t1 );
    write ( stream, t2 );
    write ( stream, t3 );
  }
  template < class Stream, class T1, class T2, class T3, class T4 > static void write ( Stream &stream, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4 )
  {
    write ( stream, t1 );
    write ( stream, t2 );
    write ( stream, t3 );
    write ( stream, t4 );
  }
  template < class Stream, class T > static void write2 ( Stream &stream, const T &t )
  {
    Usul::IO::Binary::Writer<EndianPolicy>::write ( stream, t[0], t[1] );
  }
  template < class Stream, class T > static void write3 ( Stream &stream, const T &t )
  {
    Usul::IO::Binary::Writer<EndianPolicy>::write ( stream, t[0], t[1], t[2] );
  }
  template < class Stream, class T > static void write4 ( Stream &stream, const T &t )
  {
    Usul::IO::Binary::Writer<EndianPolicy>::write ( stream, t[0], t[1], t[2], t[3] );
  }
  template < class Stream, class T > static void vector ( Stream &stream, const T &v )
  {
    // Only works properly when not converting to a particular endian.
    USUL_ASSERT_SAME_TYPE ( Usul::Endian::FromSystemToSystem, EndianPolicy );

    typedef typename T::value_type ValueType;

    if ( false == v.empty() )
    {
      stream.write ( reinterpret_cast < const char * > ( &v[0] ), sizeof ( ValueType ) * v.size() );
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Convenient typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Writer < Usul::Endian::FromSystemToBig >    WriteBigEndian;
typedef Writer < Usul::Endian::FromSystemToLittle > WriteLittleEndian;
typedef Writer < Usul::Endian::FromSystemToSystem > WriteSystemEndian;


} // namespace Binary
} // namespace IO
} // namespace Usul


#endif // _USUL_IO_BINARY_WRITER_H_
