
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For writing text at maximum floating point precision (where appropriate).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_IO_TEXT_WRITER_H_
#define _USUL_IO_TEXT_WRITER_H_

#include "Usul/Scope/StreamState.h"

#include <iostream>


namespace Usul {
namespace IO {
namespace Text {

struct Writer;

///////////////////////////////////////////////////////////////////////////////
//
//  Generic helper writer is intentionally not defined.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T > struct WriterType
  {
    static void write ( std::ostream &, const T & );
  };
}

  
///////////////////////////////////////////////////////////////////////////////
//
//  Text writer class that uses maximum floating point precision.
//
///////////////////////////////////////////////////////////////////////////////
  
struct Writer
{
  template < class T > static void write ( std::ostream &out, const T &t )
  {
    Usul::Scope::Stream::MaxFloatPrecision<T> precision ( out );
    Helper::WriterType<T>::write ( out, t );
  }
};
  
  
///////////////////////////////////////////////////////////////////////////////
//
//  Write a scalar.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T > inline void write1 ( std::ostream &out, const T &v )
  {
    out << v;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write a vector.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T > inline void write2 ( std::ostream &out, const T &v )
  {
    typedef Usul::IO::Text::Writer Writer;
    Writer::write ( out, v[0] ); out << ' ';
    Writer::write ( out, v[1] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write a vector.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T > inline void write3 ( std::ostream &out, const T &v )
  {
    typedef Usul::IO::Text::Writer Writer;
    Writer::write ( out, v[0] ); out << ' ';
    Writer::write ( out, v[1] ); out << ' ';
    Writer::write ( out, v[2] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write a vector.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T > inline void write4 ( std::ostream &out, const T &v )
  {
    typedef Usul::IO::Text::Writer Writer;
    Writer::write ( out, v[0] ); out << ' ';
    Writer::write ( out, v[1] ); out << ' ';
    Writer::write ( out, v[2] ); out << ' ';
    Writer::write ( out, v[3] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write a matrix.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T > inline void write44 ( std::ostream &out, const T &m )
  {
    typedef Usul::IO::Text::Writer Writer;
    Writer::write ( out, m ( 0, 0 ) ); out << ' ';
    Writer::write ( out, m ( 0, 1 ) ); out << ' ';
    Writer::write ( out, m ( 0, 2 ) ); out << ' ';
    Writer::write ( out, m ( 0, 3 ) ); out << ' ';
    Writer::write ( out, m ( 1, 0 ) ); out << ' ';
    Writer::write ( out, m ( 1, 1 ) ); out << ' ';
    Writer::write ( out, m ( 1, 2 ) ); out << ' ';
    Writer::write ( out, m ( 1, 3 ) ); out << ' ';
    Writer::write ( out, m ( 2, 0 ) ); out << ' ';
    Writer::write ( out, m ( 2, 1 ) ); out << ' ';
    Writer::write ( out, m ( 2, 2 ) ); out << ' ';
    Writer::write ( out, m ( 2, 3 ) ); out << ' ';
    Writer::write ( out, m ( 3, 0 ) ); out << ' ';
    Writer::write ( out, m ( 3, 1 ) ); out << ' ';
    Writer::write ( out, m ( 3, 2 ) ); out << ' ';
    Writer::write ( out, m ( 3, 3 ) );
  }
}


} // namespace Text
} // namespace IO
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to make scalar writer helper.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_IO_TEXT_DEFINE_WRITER_TYPE_SCALAR(the_type) \
namespace Usul { namespace IO { namespace Text { namespace Helper \
{ \
  template <> struct WriterType < the_type > \
  { \
    static void write ( std::ostream &out, const the_type &v ) \
    { \
      Helper::write1 ( out, v ); \
    } \
  }; \
} } } }


///////////////////////////////////////////////////////////////////////////////
//
//  Declare known scalare.
//
///////////////////////////////////////////////////////////////////////////////

USUL_IO_TEXT_DEFINE_WRITER_TYPE_SCALAR ( char );
USUL_IO_TEXT_DEFINE_WRITER_TYPE_SCALAR ( short );
USUL_IO_TEXT_DEFINE_WRITER_TYPE_SCALAR ( int );
USUL_IO_TEXT_DEFINE_WRITER_TYPE_SCALAR ( long );
USUL_IO_TEXT_DEFINE_WRITER_TYPE_SCALAR ( long long );

USUL_IO_TEXT_DEFINE_WRITER_TYPE_SCALAR ( unsigned char );
USUL_IO_TEXT_DEFINE_WRITER_TYPE_SCALAR ( unsigned short );
USUL_IO_TEXT_DEFINE_WRITER_TYPE_SCALAR ( unsigned int );
USUL_IO_TEXT_DEFINE_WRITER_TYPE_SCALAR ( unsigned long );
USUL_IO_TEXT_DEFINE_WRITER_TYPE_SCALAR ( unsigned long long );

USUL_IO_TEXT_DEFINE_WRITER_TYPE_SCALAR ( float );
USUL_IO_TEXT_DEFINE_WRITER_TYPE_SCALAR ( double );
USUL_IO_TEXT_DEFINE_WRITER_TYPE_SCALAR ( long double );


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to make vector writer helper.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_2(the_type) \
namespace Usul { namespace IO { namespace Text { namespace Helper \
{ \
  template <> struct WriterType < the_type > \
  { \
    static void write ( std::ostream &out, const the_type &v ) \
    { \
      Helper::write2 ( out, v ); \
    } \
  }; \
} } } }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to make vector writer helper.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_3(the_type) \
namespace Usul { namespace IO { namespace Text { namespace Helper \
{ \
  template <> struct WriterType < the_type > \
  { \
    static void write ( std::ostream &out, const the_type &v ) \
    { \
      Helper::write3 ( out, v ); \
    } \
  }; \
} } } }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to make vector writer helper.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_4(the_type) \
namespace Usul { namespace IO { namespace Text { namespace Helper \
{ \
  template <> struct WriterType < the_type > \
  { \
    static void write ( std::ostream &out, const the_type &v ) \
    { \
      Helper::write4 ( out, v ); \
    } \
  }; \
} } } }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to make matrix writer helper.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_IO_TEXT_DEFINE_WRITER_TYPE_MATRIX_4_4(the_type) \
namespace Usul { namespace IO { namespace Text { namespace Helper \
{ \
  template <> struct WriterType < the_type > \
  { \
    static void write ( std::ostream &out, const the_type &m ) \
    { \
      Helper::write44 ( out, m ); \
    } \
  }; \
} } } }


#endif // _USUL_IO_TEXT_WRITER_H_
