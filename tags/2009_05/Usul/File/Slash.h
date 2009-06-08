
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Platform independent slash character.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_SLASH_H_
#define _USUL_FILE_SLASH_H_

#include <string>


namespace Usul {
namespace File {


#ifdef _WIN32

  const char SLASH ( '\\' );
  inline std::string slash() { return "\\"; }

#else

  const char SLASH ( '/' );
  inline std::string slash() { return "/"; }

#endif


} // namespace File
} // namespace Usul


#endif // _USUL_FILE_SLASH_H_
