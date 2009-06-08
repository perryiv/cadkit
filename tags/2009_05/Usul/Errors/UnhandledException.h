
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Traps unhandled exceptions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ERROR_UNHANDLED_EXCEPTION_HANDLER_H_
#define _USUL_ERROR_UNHANDLED_EXCEPTION_HANDLER_H_

#ifdef _MSC_VER
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
# include "windows.h"
# include "excpt.h"
#endif


namespace Usul {
namespace Errors {


template < class ActionType > struct UnhandledException
{
  typedef UnhandledException < ActionType > ThisType;

  UnhandledException()
  {
#ifdef _MSC_VER
    ::SetUnhandledExceptionFilter ( &UnhandledException::_unhandledExceptionFilter );
#endif
  }

private:

#ifdef _MSC_VER
  static long WINAPI _unhandledExceptionFilter ( EXCEPTION_POINTERS *exceptionInfo )
  {
    ActionType action ( 2475908028, false, "Error 9880426090: Unhandled exception trapped" );
    return EXCEPTION_CONTINUE_SEARCH;
  }
#endif
};


} // namespace Errors
} // namespace Usul


#endif // _USUL_ERROR_UNHANDLED_EXCEPTION_HANDLER_H_
