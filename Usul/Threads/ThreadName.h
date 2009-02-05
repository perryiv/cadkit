
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions for working with the system-thread's name.
//  Helpful when using a debugger.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_THREAD_NAME_H_
#define _USUL_THREADS_THREAD_NAME_H_

#ifdef _MSC_VER
# ifndef NOMINMAX
#  define NOMINMAX
# endif
# ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
# endif
# pragma pack ( push, 8 )
# include "windows.h"
#endif

#include <string>


namespace Usul {
namespace Threads {


#ifdef _MSC_VER


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct for setting the system-thread's name.
//  See: http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct ThreadNameInfo
  {
    DWORD _type;      // Must be 0x1000.
    LPCSTR _name;     // Pointer to name (in user addr space).
    DWORD _threadID;  // Thread ID (-1=caller thread).
    DWORD _flags;     // Reserved for future use, must be zero.
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the system thread name.
//  See: http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
//
///////////////////////////////////////////////////////////////////////////////

inline void setSystemThreadName ( const std::string &name, DWORD id )
{
  if ( false == name.empty() )
  {
    Usul::Threads::Helper::ThreadNameInfo info = { 0x1000, name.c_str(), id, 0 };
    __try
    {
      ::RaiseException ( 0x406D1388, 0, sizeof ( info ) / sizeof ( ULONG_PTR ), reinterpret_cast < ULONG_PTR * > ( &info  ) );
    }
    __except ( EXCEPTION_CONTINUE_EXECUTION )
    {
    }
  }
}


#else


inline void setSystemThreadName ( const std::string &, unsigned long )
{
}


#endif


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_THREAD_NAME_H_
