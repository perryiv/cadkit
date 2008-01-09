
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Thread-safe convenience functions and macros.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_THREAD_SAFE_OPERATIONS_H_
#define _USUL_THREADS_THREAD_SAFE_OPERATIONS_H_

#include "Usul/Threads/Guard.h"


namespace Usul {
namespace Threads {
namespace Safe {


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to block while getting the value.
//
///////////////////////////////////////////////////////////////////////////////

template < class MutexType, class T > T get ( MutexType &mutex, const T &t )
{
  typedef Usul::Threads::Guard<MutexType> Guard;
  Guard guard ( mutex );
  return t;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to block while setting the value.
//
///////////////////////////////////////////////////////////////////////////////

template < class MutexType, class T1, class T2 > void set ( MutexType &mutex, const T1 &from, T2 &to )
{
  typedef Usul::Threads::Guard<MutexType> Guard;
  Guard guard ( mutex );
  to = from;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to block while executing the function.
//
///////////////////////////////////////////////////////////////////////////////

template < class MutexType, class FunctionType > void execute ( MutexType &mutex, FunctionType f )
{
  typedef Usul::Threads::Guard<MutexType> Guard;
  Guard guard ( mutex );
  f();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience macro to block while executing the expression.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_THREADS_SAFE_EXECUTE(mutex_type,the_mutex,expression) \
{ \
  typedef Usul::Threads::Guard<mutex_type> the_guard_type; \
  the_guard_type the_guard ( the_mutex ); \
  { \
    expression; \
  } \
}


} // namespace Safe
} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_THREAD_SAFE_OPERATIONS_H_
