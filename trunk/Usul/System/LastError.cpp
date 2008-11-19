
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that represents the system's last error.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/LastError.h"
#include "Usul/MPL/SameType.h"
#include "Usul/Strings/Trim.h"
#include "Usul/Errors/Assert.h"

#ifdef _WIN32
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
# include <windows.h> // For GetLastError()
#else
# include <dlfcn.h>   // For dlerror()
#endif

#include <errno.h>   // For errno
#include <string.h>  // For strerror()
#include <algorithm>

using namespace Usul;
using namespace Usul::System;


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the last error.
//
///////////////////////////////////////////////////////////////////////////////

void LastError::init()
{
  // Initialize standard error number.
  errno = 0;

#ifdef _WIN32

  // This will make ::GetLastError() return ERROR_SUCCESS.
  ::SetLastError ( ERROR_SUCCESS );

#else

  // This will make subsequent calls to dlerror() return null.
  ::dlerror();

#endif

  USUL_ASSERT ( false == LastError::has() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of the last error.
//
///////////////////////////////////////////////////////////////////////////////

LastError::Number LastError::number()
{
#ifdef _WIN32

  USUL_ASSERT_SAME_TYPE ( DWORD, unsigned long );
  USUL_ASSERT_SAME_TYPE ( DWORD, LastError::Number );

  // Getting also initializes, so we have to immediately set again.
  const DWORD error ( ::GetLastError() );
  ::SetLastError ( error );

  // Return error.
  return error;

#else

  USUL_ASSERT_SAME_TYPE ( int, LastError::Number );
  USUL_STATIC_ASSERT ( sizeof ( int ) == sizeof ( LastError::Number ) );
  return errno;

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last error message.
//
///////////////////////////////////////////////////////////////////////////////

std::string LastError::message()
{
  return LastError::message ( LastError::number() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last error message.
//
///////////////////////////////////////////////////////////////////////////////

std::string LastError::message ( Number number )
{
#ifdef _WIN32

  // The buffer to write to. It will be allocated.
  LPVOID buffer;

  // Format the message.
  ::FormatMessage ( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
    FORMAT_MESSAGE_FROM_SYSTEM | 
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    number,
    MAKELANGID ( LANG_NEUTRAL, SUBLANG_DEFAULT ), // Default language.
    (LPTSTR) &buffer,
    0,
    NULL );

  // Copy the message.
  std::string message ( buffer ? reinterpret_cast<LPCTSTR> ( buffer ) : "" );

  // Free the buffer.
  ::LocalFree ( buffer );

  // Remove all carriage-return characters.
  message.erase ( std::remove ( message.begin(), message.end(), '\r' ), message.end() );

  // Remove all trailing new-line characters.
  Usul::Strings::trimRightAll ( message, '\n' );

  // Return the message.
  return message;

#else

  // First, see if the error is from loading a dynamic library.
  const char *buf = ::dlerror();

  // If so, then return it.
  if ( buf )
    return std::string ( buf );

  // Otherwise, get the message associated with the current error number.
  buf = ::strerror ( number );

  // Return the error message. Handle the null case.
  return std::string ( ( buf ) ? buf : "" );

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is there an error?
//
///////////////////////////////////////////////////////////////////////////////

bool LastError::has()
{
#ifdef _WIN32

  const DWORD error ( LastError::number() );
  return ( ERROR_SUCCESS != error );

#else

  return ( 0 != errno ); // Is this right? TODO

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Internal class that asserts in it's constructor and destructor 
//  if there is a system error.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  inline void checkLastError()
  {
    if ( true == Usul::System::LastError::has() )
    {
      USUL_ASSERT ( false );
      const std::string error ( Usul::System::LastError::message() );
    }
  }
}
LastError::Assert::Assert()
{
  Detail::checkLastError();
}
LastError::Assert::~Assert()
{
  Detail::checkLastError();
}
