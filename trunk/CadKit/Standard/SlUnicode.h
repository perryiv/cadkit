
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  SlUnicode: Inline functions for unicode.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_INLINE_UNICODE_H_
#define _CADKIT_STANDARD_LIBRARY_INLINE_UNICODE_H_

#include "SlAssert.h"

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX            // Do not define min and max as macros.
# include <windows.h>
#endif // _WIN32
#include <string>

namespace CadKit 
{
///////////////////////////////////////////////////////////////////////////////
//
// Converts the ANSI string chars to a Unicode string and returns the Unicode 
// string through wchars. The integers are the sizes of the arrays, not the 
// string length (i.e., include the NULL termination character in the size).
// 
///////////////////////////////////////////////////////////////////////////////

inline HRESULT ansiToUnicode ( const unsigned int &csize, const char *chars, const unsigned int &wsize, wchar_t *wchars )
{
  SL_ASSERT ( csize > 0 );
  SL_ASSERT ( wsize > 0 );
  SL_ASSERT ( 0x0 != chars );
  SL_ASSERT ( 0x0 != wchars );
  SL_ASSERT ( wsize >= csize );                  // Enough room?
  SL_ASSERT ( csize == 1 + ::strlen ( chars ) ); // Include NULL character.

#ifdef _WIN32

  // Covert to Unicode.
  if ( 0 == ::MultiByteToWideChar ( CP_ACP, 0, chars, csize, wchars, wsize ) )
  {
    DWORD error = ::GetLastError();
    return HRESULT_FROM_WIN32 ( error );
  }

#elif __GNUC__

  SL_ASSERT ( 0 ); // TODO.

#else
  TODO
#endif

  // Since we went from ansi to unicode, each ansi character becomes a single 
  // unicode character. Therefore, the string lengths should be the same.
  SL_ASSERT ( csize == 1 + ::wcslen ( wchars ) );

  // It worked.
  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Converts the Unicode string wchars to an ANSI string and returns the ANSI 
// string through chars. The integers are the sizes of the arrays, not the 
// string length (i.e., include the NULL termination character in the size).
// 
///////////////////////////////////////////////////////////////////////////////

inline HRESULT unicodeToAnsi ( const unsigned int &wsize, const wchar_t *wchars, const unsigned int &csize, char *chars )
{
  SL_ASSERT ( csize > 0 );
  SL_ASSERT ( wsize > 0 );
  SL_ASSERT ( 0x0 != chars );
  SL_ASSERT ( 0x0 != wchars );
  SL_ASSERT ( csize >= 2 * wsize );               // Enough room?
  SL_ASSERT ( wsize == 1 + ::wcslen ( wchars ) ); // Include NULL character.

#ifdef _WIN32

  // Convert to ANSI.
  if ( 0 == ::WideCharToMultiByte ( CP_ACP, 0, wchars, wsize, chars, csize, 0x0, 0x0 ) )
  {
    DWORD error = ::GetLastError();
    return HRESULT_FROM_WIN32 ( error );
  }

#elif __GNUC__

  SL_ASSERT ( 0 ); // TODO.

#else
  TODO
#endif

  // Converting from unicode to ansi will result in a char array that is at 
  // most twice as long as the wchar array (cause each wchar may need 2 chars).
  // However, sometimes (like if the wchar is really an ansi char) a wchar will
  // convert to a single char. Therefore, the char array should be at least as 
  // big as the wchar array, but no bigger than twice as big as the wchar array.
  SL_ASSERT ( 1 + ::strlen ( chars ) >= wsize );
  SL_ASSERT ( 1 + ::strlen ( chars ) <= csize );

  // It worked.
  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Converts the ANSI string chars to a Unicode string and returns the Unicode 
// string through wchars.
// 
///////////////////////////////////////////////////////////////////////////////

inline HRESULT ansiToUnicode ( const std::string &chars, std::wstring &wchars )
{
  // Handle trivial case.
  if ( 0 == chars.size() )
  {
    wchars = L"";
    return S_OK;
  }

  // The wide array is the same number of characters as the single array.
  // Include the NULL character in the size.
  unsigned int wsize = chars.size() + 1;
  wchar_t *wtemp = new wchar_t[wsize];
  if ( 0x0 == wtemp ) return E_OUTOFMEMORY;

  // Call the other one.
  HRESULT hr = ansiToUnicode ( chars.size() + 1, chars.c_str(), wsize, wtemp );

  // The new wide character array should be same length as single byte array.
  SL_ASSERT ( ::wcslen ( wtemp ) == chars.size() );
  SL_ASSERT ( ::wcslen ( wtemp ) == wsize - 1 );

  // Copy to the given wide string.
  if ( SUCCEEDED ( hr ) ) wchars = wtemp;

  // Done with this.
  delete [] wtemp;

  // Return the result;
  return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Converts the Unicode string wchars to an ANSI string and returns the ANSI 
// string through chars.
// 
///////////////////////////////////////////////////////////////////////////////

inline HRESULT unicodeToAnsi ( const std::wstring &wchars, std::string &chars )
{
  // Handle trivial case.
  if ( 0 == wchars.size() )
  {
    chars = "";
    return S_OK;
  }

  // We may need up to twice as many single characters to hold all the wide 
  // characters. Include the NULL character in the size.
  unsigned int csize = 2 * ( wchars.size() + 1 );
  char *ctemp = new char[csize];
  if ( 0x0 == ctemp ) return E_OUTOFMEMORY;

  // Call the other one.
  HRESULT hr = unicodeToAnsi ( wchars.size() + 1, wchars.c_str(), csize, ctemp );

  // The length of the new single byte array should be greater than or equal 
  // to the number of wide characters, but less than or equal to the number of
  // single characters we allocated.
  SL_ASSERT ( ::strlen ( ctemp ) >= wchars.size() );
  SL_ASSERT ( ::strlen ( ctemp ) <= csize - 1 );

  // Copy to the given wide string.
  if ( SUCCEEDED ( hr ) ) chars = ctemp;

  // Done with this.
  delete [] ctemp;

  // Return the result;
  return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Converts the ANSI string chars to a Unicode string and returns the Unicode 
// string through wchars.
// 
///////////////////////////////////////////////////////////////////////////////

inline HRESULT ansiToUnicode ( const char *chars, const unsigned int &wsize, wchar_t *wchars )
{
  SL_ASSERT ( wsize > 0 );
  SL_ASSERT ( 0x0 != chars );
  SL_ASSERT ( 0x0 != wchars );

  // Call the other one.
  return ansiToUnicode ( ::strlen ( chars ) + 1, chars, wsize, wchars );
}


///////////////////////////////////////////////////////////////////////////////
//
// Converts the Unicode string wchars to an ANSI string and returns the ANSI 
// string through chars.
// 
///////////////////////////////////////////////////////////////////////////////

inline HRESULT unicodeToAnsi ( const wchar_t *wchars, const unsigned int &csize, char *chars )
{
  SL_ASSERT ( csize > 0 );
  SL_ASSERT ( 0x0 != chars );
  SL_ASSERT ( 0x0 != wchars );

  // Call the other one.
  return unicodeToAnsi ( ::wcslen ( wchars ) + 1, wchars, csize, chars );
}

}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_INLINE_UNICODE_H_
