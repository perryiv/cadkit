
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that throws exceptions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_EXCEPTIONS_THROWER_CLASS_H_
#define _USUL_EXCEPTIONS_THROWER_CLASS_H_

#include "Usul/Headers/SStream.h"


namespace Usul {
namespace Exceptions {


template < class ExceptionType_ > class Thrower
{
public:

  typedef ExceptionType_ ExceptionType;

  // Default constructor.
  Thrower()
  {
    throw ExceptionType();
  }

  // Constructor.
  template < class T >
  Thrower ( const T &t )
  {
    std::ostringstream message;
    message << t;
    throw ExceptionType ( message.str() );
  }

  // Constructor.
  template < class T1, class T2 >
  Thrower ( const T1 &t1, const T2 &t2 )
  {
    std::ostringstream message;
    message << t1 << t2;
    throw ExceptionType ( message.str() );
  }

  // Constructor.
  template < class T1, class T2, class T3 >
  Thrower ( const T1 &t1, const T2 &t2, const T3 &t3 )
  {
    std::ostringstream message;
    message << t1 << t2 << t3;
    throw ExceptionType ( message.str() );
  }

  // Constructor.
  template < class T1, class T2, class T3, class T4 >
  Thrower ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4 )
  {
    std::ostringstream message;
    message << t1 << t2 << t3 << t4;
    throw ExceptionType ( message.str() );
  }

  // Constructor.
  template < class T1, class T2, class T3, class T4, class T5 >
  Thrower ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5 )
  {
    std::ostringstream message;
    message << t1 << t2 << t3 << t4 << t5;
    throw ExceptionType ( message.str() );
  }

  // Constructor.
  template < class T1, class T2, class T3, class T4, class T5, class T6 >
  Thrower ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6 )
  {
    std::ostringstream message;
    message << t1 << t2 << t3 << t4 << t5 << t6;
    throw ExceptionType ( message.str() );
  }

  // Constructor.
  template < class T1, class T2, class T3, class T4, class T5, class T6, class T7 >
  Thrower ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7 )
  {
    std::ostringstream message;
    message << t1 << t2 << t3 << t4 << t5 << t6 << t7;
    throw ExceptionType ( message.str() );
  }

  // Constructor.
  template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8 >
  Thrower ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8 )
  {
    std::ostringstream message;
    message << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8;
    throw ExceptionType ( message.str() );
  }

  // Constructor.
  template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9 >
  Thrower ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9 )
  {
    std::ostringstream message;
    message << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9;
    throw ExceptionType ( message.str() );
  }

  // Constructor.
  template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10 >
  Thrower ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10 )
  {
    std::ostringstream message;
    message << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10;
    throw ExceptionType ( message.str() );
  }

  // Constructor.
  template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11 >
  Thrower ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11 )
  {
    std::ostringstream message;
    message << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11;
    throw ExceptionType ( message.str() );
  }

  // Constructor.
  template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12 >
  Thrower ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12 )
  {
    std::ostringstream message;
    message << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12;
    throw ExceptionType ( message.str() );
  }

  // Constructor.
  template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13 >
  Thrower ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13 )
  {
    std::ostringstream message;
    message << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13;
    throw ExceptionType ( message.str() );
  }

  // Constructor.
  template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14 >
  Thrower ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14 )
  {
    std::ostringstream message;
    message << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14;
    throw ExceptionType ( message.str() );
  }
};


} // namespace Exceptions
} // namespace Usul


#endif // _USUL_EXCEPTIONS_THROWER_CLASS_H_
