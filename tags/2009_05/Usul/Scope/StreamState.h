
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Sets and resets the stream state.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SCOPED_STREAM_STATE_H_
#define _USUL_SCOPED_STREAM_STATE_H_

#include <iomanip>
#include <limits>
#include <ostream>

namespace Usul {
namespace Scope {
namespace Stream {


///////////////////////////////////////////////////////////////////////////////
//
//  Set/reset the output stream flags.
//
///////////////////////////////////////////////////////////////////////////////

struct Flags
{
  Flags ( std::ostream &out, std::ios_base::fmtflags flags, std::ios_base::fmtflags mask ) :
    _out ( out ),
    _flags ( out.setf ( flags, mask ) ),
    _mask ( mask ),
    _useMask ( true )
  {
  }

  Flags ( std::ostream &out, std::ios_base::fmtflags flags ) :
    _out ( out ),
    _flags ( out.setf ( flags ) ),
    _mask ( static_cast < std::ios_base::fmtflags > ( 0 ) ),
    _useMask ( false )
  {
  }

  ~Flags()
  {
    if ( _useMask )
    {
      _out.setf ( _flags, _mask );
    }
    else
    {
      _out.setf ( _flags );
    }
  }

private:

  Flags ( const Flags & );
  Flags &operator = ( const Flags & );

  std::ostream &_out;
  std::ios_base::fmtflags _flags;
  std::ios_base::fmtflags _mask;
  bool _useMask;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Set/reset the output stream's precision.
//
///////////////////////////////////////////////////////////////////////////////

struct Precision
{
  Precision ( std::ostream &out, std::streamsize p ) :
    _out ( out ),
    _precision ( out.precision ( p ) )
  {
  }

  ~Precision()
  {
    _out.precision ( _precision );
  }

private:

  Precision ( const Precision & );
  Precision &operator = ( const Precision & );

  std::ostream &_out;
  std::streamsize _precision;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Set/reset the stream for maximum precision. General case does nothing.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct MaxFloatPrecision
{
  MaxFloatPrecision ( std::ostream & )
  {
  }
  ~MaxFloatPrecision()
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to define helper class to set/reset the stream to maximum precision.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_SCOPE_STREAM_DEFINE_MAX_FLOAT_PRECISION(the_type) \
template <> struct MaxFloatPrecision < the_type > \
{ \
  MaxFloatPrecision ( std::ostream &out ) : \
    _flags ( out, std::ios_base::fixed, std::ios_base::floatfield ), \
    _precision ( out, std::numeric_limits<the_type>::digits10 ) \
  { \
  } \
  ~MaxFloatPrecision() \
  { \
  } \
private: \
  MaxFloatPrecision ( const MaxFloatPrecision & ); \
  MaxFloatPrecision &operator = ( const MaxFloatPrecision & ); \
  Flags _flags; \
  Precision _precision; \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Define helper classes for known float types.
//
///////////////////////////////////////////////////////////////////////////////

USUL_SCOPE_STREAM_DEFINE_MAX_FLOAT_PRECISION ( float );
USUL_SCOPE_STREAM_DEFINE_MAX_FLOAT_PRECISION ( double );
USUL_SCOPE_STREAM_DEFINE_MAX_FLOAT_PRECISION ( long double );


} // namespace Stream
} // namespace Scope
} // namespace Usul


#endif // _USUL_SCOPED_STREAM_STATE_H_
