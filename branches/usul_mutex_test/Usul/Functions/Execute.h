
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions to execute other functions if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTIONS_EXECUTE_H_
#define _USUL_FUNCTIONS_EXECUTE_H_

#include <functional>


namespace Usul {
namespace Functions {


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute another function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class F > 
inline void executeFunction ( F f )
{
  f ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute another function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class F, class A1 >
inline void executeFunction ( F f, A1 a1 )
{
  f ( a1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute another function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class F, class A1, class A2 >
inline void executeFunction ( F f, A1 a1, A2 a2 )
{
  f ( a1, a2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute another function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class F, class A1, class A2, class A3 >
inline void executeFunction ( F f, A1 a1, A2 a2, A3 a3 )
{
  f ( a1, a2, a3 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute a member-function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class P, class F >
inline void executeMemberFunction ( P *ptr, F f )
{
  if ( 0x0 != ptr )
  {
    ((*ptr).*f)();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute a member-function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class P, class F, class A1 >
inline void executeMemberFunction ( P *ptr, F f, A1 a1 )
{
  if ( 0x0 != ptr )
  {
    ((*ptr).*f) ( a1 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute a member-function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class P, class F, class A1, class A2 >
inline void executeMemberFunction ( P *ptr, F f, A1 a1, A2 a2 )
{
  if ( 0x0 != ptr )
  {
    ((*ptr).*f) ( a1, a2 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute a member-function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class P, class F, class A1, class A2, class A3 >
inline void executeMemberFunction ( P *ptr, F f, A1 a1, A2 a2, A3 a3 )
{
  if ( 0x0 != ptr )
  {
    ((*ptr).*f) ( a1, a2, a3 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute a member-function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class P, class F, class A1, class A2, class A3, class A4 >
inline void executeMemberFunction ( P *ptr, F f, A1 a1, A2 a2, A3 a3, A4 a4 )
{
  if ( 0x0 != ptr )
  {
    ((*ptr).*f) ( a1, a2, a3, a4 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute another function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class C, class F > 
inline void executeFunctions ( C &c, F f )
{
  for ( typename C::iterator i = c.begin(); i != c.end(); ++i )
  {
    Usul::Functions::executeFunction ( i->get(), f );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute another function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class C, class F, class A1 > 
inline void executeFunctions ( C &c, F f, A1 a1 )
{
  for ( typename C::iterator i = c.begin(); i != c.end(); ++i )
  {
    Usul::Functions::executeFunction ( i->get(), f, a1 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute another function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class C, class F, class A1, class A2 > 
inline void executeFunctions ( C &c, F f, A1 a1, A2 a2 )
{
  for ( typename C::iterator i = c.begin(); i != c.end(); ++i )
  {
    Usul::Functions::executeFunction ( i->get(), f, a1, a2 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute another function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class C, class F, class A1, class A2, class A3 > 
inline void executeFunctions ( C &c, F f, A1 a1, A2 a2, A3 a3 )
{
  for ( typename C::iterator i = c.begin(); i != c.end(); ++i )
  {
    Usul::Functions::executeFunction ( i->get(), f, a1, a2, a3 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute another function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class C, class F > 
inline void executeMemberFunctions ( C &c, F f )
{
  for ( typename C::iterator i = c.begin(); i != c.end(); ++i )
  {
    Usul::Functions::executeMemberFunction ( i->get(), f );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute another function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class C, class F, class A1 > 
inline void executeMemberFunctions ( C &c, F f, A1 a1 )
{
  for ( typename C::iterator i = c.begin(); i != c.end(); ++i )
  {
    Usul::Functions::executeMemberFunction ( i->get(), f, a1 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute another function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class C, class F, class A1, class A2 > 
inline void executeMemberFunctions ( C &c, F f, A1 a1, A2 a2 )
{
  for ( typename C::iterator i = c.begin(); i != c.end(); ++i )
  {
    Usul::Functions::executeMemberFunction ( i->get(), f, a1, a2 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute another function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class C, class F, class A1, class A2, class A3 > 
inline void executeMemberFunctions ( C &c, F f, A1 a1, A2 a2, A3 a3 )
{
  for ( typename C::iterator i = c.begin(); i != c.end(); ++i )
  {
    Usul::Functions::executeMemberFunction ( i->get(), f, a1, a2, a3 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to execute another function if the conditions are right.
//
///////////////////////////////////////////////////////////////////////////////

template < class C, class F, class A1, class A2, class A3, class A4 > 
inline void executeMemberFunctions ( C &c, F f, A1 a1, A2 a2, A3 a3, A4 a4 )
{
  for ( typename C::iterator i = c.begin(); i != c.end(); ++i )
  {
    Usul::Functions::executeMemberFunction ( i->get(), f, a1, a2, a3, a4 );
  }
}


} // namespace Functions
} // namespace Usul


#endif // _USUL_FUNCTIONS_EXECUTE_H_
