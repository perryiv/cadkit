
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Platform independent newline character.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_LINE_ENDING_H_
#define _USUL_FILE_LINE_ENDING_H_

#include <string>


namespace Usul {
namespace File {
    
    
#ifdef _WIN32
    
  const char* LINE_ENDING ( "\r\n'" );
    
#else
    
  const char* LINE_ENDING ( "\n" );
    
#endif
    
  inline std::string lineEnding() { return std::string ( LINE_ENDING ); }

} // namespace File
} // namespace Usul


#endif // _USUL_FILE_LINE_ENDING_H_
