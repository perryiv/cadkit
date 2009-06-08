
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Convert managed string to native.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_STRINGS_MANAGED_TO_NATIVE_
#define _USUL_STRINGS_MANAGED_TO_NATIVE_

#include <string>

namespace Usul {
namespace Strings {


inline std::string convert ( ::System::String^ source )
{
  std::string answer;
  ::System::IntPtr ptr ( 0 );
  try
  {
    ptr = ::System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( source );
    char* s = (char*)(void*) ptr;
    answer.assign ( s );
  }
  finally
  {
    ::System::Runtime::InteropServices::Marshal::FreeHGlobal( ptr );
  }
  return answer;
}


} // String
} // Usul


#endif // _USUL_STRINGS_MANAGED_TO_NATIVE_
