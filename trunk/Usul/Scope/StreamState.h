
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
    _mask ( 0 ),
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
//  Set/reset the output stream for maximum precision.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct MaxPrecision
{
  MaxPrecision ( std::ostream &out ) :
    _precision ( out, std::numeric_limits<T>::digits10 )
  {
  }

  ~MaxPrecision()
  {
  }

private:

  MaxPrecision ( const MaxPrecision & );
  MaxPrecision &operator = ( const MaxPrecision & );

  Precision _precision;
};


} // namespace Stream
} // namespace Scope
} // namespace Usul


#endif // _USUL_SCOPED_STREAM_STATE_H_
