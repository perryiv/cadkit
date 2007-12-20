
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Traps invalid parameter calls in Windows CRT.
//  See http://msdn2.microsoft.com/en-us/library/a9yf33zb(VS.80).aspx
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ERROR_INVALID_PARAMETER_HANDLER_H_
#define _USUL_ERROR_INVALID_PARAMETER_HANDLER_H_

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include "Usul/Strings/Format.h"
#include "Usul/Strings/Unicode.h"


namespace Usul {
namespace Errors {


template < class ActionType > struct InvalidParameter
{
  typedef InvalidParameter < ActionType > ThisType;

  InvalidParameter()
  {
    #ifdef _MSC_VER
    ::_set_invalid_parameter_handler ( &ThisType::_invalidParameterHandler );
    #endif
  }

private:

  static void _invalidParameterHandler ( 
    const wchar_t *expression,
    const wchar_t *function, 
    const wchar_t *file, 
    unsigned int line,
    uintptr_t pReserved )
  {
    const std::string message ( Usul::Strings::format ( 
      "Error 5100321580: Invalid parameter used with CRT function",
      ( ( 0x0 == expression ) ? "" : Usul::Strings::format ( "\n  Expression: ", Usul::Strings::Unicode::convert ( expression ) ) ),
      ( ( 0x0 == function )   ? "" : Usul::Strings::format ( "\n  Function: ",   Usul::Strings::Unicode::convert ( function ) ) ),
      ( ( 0x0 == file )       ? "" : Usul::Strings::format ( "\n  File: ",       Usul::Strings::Unicode::convert ( file ) ) ),
      ( (   0 == line )       ? "" : Usul::Strings::format ( "\n  Line: ",       line ) ) ) );
    ActionType action ( 3327592610u, false, message );
  }
};


} // namespace Errors
} // namespace Usul


#endif // _USUL_ERROR_INVALID_PARAMETER_HANDLER_H_
