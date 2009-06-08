
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlStringCase: For changing the case of a string.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _CADKIT_STANDARD_LIBRARY_STRING_CASE_FUNCTIONS_H_
#define _CADKIT_STANDARD_LIBRARY_STRING_CASE_FUNCTIONS_H_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
# include <algorithm> // For std::transform
# include <ctype.h>   // For towupper() and towlower()
# include <wctype.h>  // For towupper() and towlower()
#endif


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Make the string all upper/lower case.
//
//  The simple way is to use the current locale, found at:
//  http://sunsite.ualberta.ca/Documentation/Gnu/libstdc++-2.90.8/html/21_strings/howto.html#4
//
//  Another way to do it, which lets the client pass in the locale, is at:
//  http://gcc.gnu.org/onlinedocs/libstdc++/22_locale/howto.html
//  I wasn't able to get that method to compile on all platforms.
//
///////////////////////////////////////////////////////////////////////////////

inline void toUpper ( std::basic_string<char> &s )
{
  std::transform ( s.begin(), s.end(), s.begin(), ::toupper );
}

inline void toUpper ( std::basic_string<wchar_t> &s )
{
  std::transform ( s.begin(), s.end(), s.begin(), ::towupper );
}

inline void toLower ( std::basic_string<char> &s )
{
  std::transform ( s.begin(), s.end(), s.begin(), ::tolower );
}

inline void toLower ( std::basic_string<wchar_t> &s )
{
  std::transform ( s.begin(), s.end(), s.begin(), ::towlower );
}


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_STRING_CASE_FUNCTIONS_H_
