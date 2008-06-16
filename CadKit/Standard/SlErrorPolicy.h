
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlErrorPolicy: A collection of small error-policy classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_ERROR_POLICIES_H_
#define _CADKIT_STANDARD_LIBRARY_ERROR_POLICIES_H_

// Fix for gcc 4.1 compile error
#ifndef _WIN32
#include <cassert>
#endif

namespace CadKit {
namespace ErrorPolicy {


///////////////////////////////////////////////////////////////////////////////
//
//  A policy that throws an exception.
//
///////////////////////////////////////////////////////////////////////////////

template < class Exception > class Throw
{
  template < class Argument > void _throw ( bool state, const Argument &argument )
  {
    if ( !state )
      throw Exception ( argument );
  }
public:
  Throw(){}
  template < class Argument > Throw ( bool state, const Argument &argument )
  {
    this->_throw ( state, argument );
  }
  template < class Argument > void operator() ( bool state, const Argument &argument )
  {
    this->_throw ( state, argument );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  A policy that does nothing.
//
///////////////////////////////////////////////////////////////////////////////

struct DoNothing
{
  DoNothing(){}
  template < class Argument > void operator() ( bool state, const Argument &argument ){}
};


///////////////////////////////////////////////////////////////////////////////
//
//  A policy that prints to the specified stream.
//
///////////////////////////////////////////////////////////////////////////////

template < class OutputStream > struct Print
{
  Print ( OutputStream &out ) : _out ( out ){}
  template < class Argument > void operator() ( bool state, const Argument &argument )
  {
    if ( !state )
      _out << argument;
  }
private:
  OutputStream &_out;
};


///////////////////////////////////////////////////////////////////////////////
//
//  A policy that calls the given function (or functor).
//
///////////////////////////////////////////////////////////////////////////////

template < class Function > struct Call
{
  Call ( const Function &f ) : _f ( f ){}
  template < class Argument > void operator() ( bool state, const Argument &argument )
  {
    if ( !state )
      _f ( argument );
  }
private:
  Function _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  A policy that asserts.
//
///////////////////////////////////////////////////////////////////////////////

struct Assert
{
  Assert(){}
  template < class Argument > void operator() ( bool state, const Argument &argument )
  {
    assert ( state );
  }
};


}; // namespace ErrorPolicy
}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_ERROR_POLICIES_H_
