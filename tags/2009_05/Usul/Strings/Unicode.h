
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions for unicode conversions. 
//  See http://msdn2.microsoft.com/en-us/library/ms235631(VS.80).aspx
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_STRINGS_UNICODE_H_
#define _USUL_STRINGS_UNICODE_H_

#include <cstdlib>
#include <string>
#include <vector>


namespace Usul {
namespace Strings {
namespace Unicode {


inline void convert ( const std::string &s, std::wstring &w )
{
  size_t numConvertedChars ( 0 );
  std::vector<wchar_t> target ( s.size() + 1 );
  #ifdef _MSC_VER
  ::mbstowcs_s ( &numConvertedChars, &target[0], target.size(), s.c_str(), _TRUNCATE );
  #else
  numConvertedChars = ::mbstowcs ( &target[0], s.c_str(), target.size() );
  #endif
  w = &target[0];
}

inline void convert ( const std::wstring &w, std::string &s )
{
  size_t numConvertedChars ( 0 );
  std::vector<char> target ( 2 * w.size() + 1 );
  #ifdef _MSC_VER
  ::wcstombs_s ( &numConvertedChars, &target[0], target.size(), w.c_str(), _TRUNCATE );
  #else
  numConvertedChars = ::wcstombs ( &target[0], w.c_str(), target.size() );
  #endif
  s = &target[0];
}

inline void convert ( const char *s, std::wstring &w )
{
  if ( 0x0 != s )
  {
    Usul::Strings::Unicode::convert ( std::string ( s ), w );
  }
}

inline void convert ( const wchar_t *w, std::string &s )
{
  if ( 0x0 != w )
  {
    Usul::Strings::Unicode::convert ( std::wstring ( w ), s );
  }
}

inline std::wstring convert ( const std::string &s )
{
  std::wstring w;
  Usul::Strings::Unicode::convert ( s, w );
  return w;
}

inline std::string convert ( const std::wstring &w )
{
  std::string s;
  Usul::Strings::Unicode::convert ( w, s );
  return s;
}

inline std::wstring convert ( const char *s )
{
  std::wstring w;
  Usul::Strings::Unicode::convert ( s, w );
  return w;
}

inline std::string convert ( const wchar_t *w )
{
  std::string s;
  Usul::Strings::Unicode::convert ( w, s );
  return s;
}


} // namespace Unicode
} // namespace Strings
} // namespace Usul


#endif // _USUL_STRINGS_UNICODE_H_
