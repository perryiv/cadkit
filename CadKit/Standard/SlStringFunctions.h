
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

///////////////////////////////////////////////////////////////////////////////
//
//  SlStringFunctions: A collection of functions that work with basic_string 
//  and/or SlString. They are in this file (as opposed to in SlString) because 
//  they introduce dependencies on specific data types.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _CADKIT_STANDARD_LIBRARY_STRING_FUNCTIONS_H_
#define _CADKIT_STANDARD_LIBRARY_STRING_FUNCTIONS_H_

#include "SlFunctionDefines.h"
#include "SlAssert.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# ifdef _WIN32
#  include "windows.h"
# endif
# include <stdarg.h>
# include <stdlib.h>
# include <wchar.h>
# include <string>
# include <list>
# include <algorithm> // For std::transform
#endif

#define SL_STRING_FUNCTION_BUFFER_SIZE 32768 // 2^15


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


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given string is equal to this string.
//
///////////////////////////////////////////////////////////////////////////////

template<class String> inline bool isEqual ( const String &s1, const String &s2, bool caseInsensitive )
{
  // If we are doing case-insensitive...
  if ( caseInsensitive )
  {
    // Compare the upper-case copies.
    String a ( s1 ), b ( s2 );
    CadKit::toUpper ( a );
    CadKit::toUpper ( b );
    return ( a == b );
  }

  // Otherwise, compare them as-is.
  return ( s1 == s2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Format the string.
//
///////////////////////////////////////////////////////////////////////////////

inline void format ( const char *f, va_list ap, std::basic_string<char> &str )
{
   SL_ASSERT ( f );
   SL_ASSERT ( ap );

   // The string we write to. Do not make these static because I want it 
   // to be thread-safe. Hopefully this is big enough.
   const size_t length = SL_STRING_FUNCTION_BUFFER_SIZE - 1; // 2^15 - 1.
   char buf[length + 1];

   // Format the text into the buffer.
   SL_VSNPRINTF ( buf, length, f, ap );

   // Check for memory problems.
   SL_ASSERT ( ::strlen ( buf ) <= length );

   // Set the new string.
   str = buf;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Format the string.
//
///////////////////////////////////////////////////////////////////////////////

inline void format ( const wchar_t *f, va_list ap, std::basic_string<wchar_t> &str )
{
   SL_ASSERT ( f );
   SL_ASSERT ( ap );

   // The string we write to. Do not make these static because I want it 
   // to be thread-safe. Hopefully this is big enough.
   const size_t length = SL_STRING_FUNCTION_BUFFER_SIZE - 1; // 2^15 - 1.
   wchar_t buf[length + 1];

   // Format the text into the buffer.
   SL_VSNWPRINTF ( buf, length, f, ap );

   // Check for memory problems.
   SL_ASSERT ( ::wcslen ( buf ) <= length );

   // Set the new string.
   str = buf;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Format the string.
//
///////////////////////////////////////////////////////////////////////////////

template<class Char, class String> inline void format ( String &str, const Char *f, ... )
{
   SL_ASSERT ( f );

   // Start variable argument processing.
   va_list ap;
   va_start ( ap, f );

   // Format the text.
   CadKit::format ( f, ap, str );

   // End variable argument processing.
   va_end ( ap );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Format the string.
//
///////////////////////////////////////////////////////////////////////////////

template<class Char> inline std::basic_string<Char> format ( const Char *f, ... )
{
   SL_ASSERT ( f );

   // Start variable argument processing.
   va_list ap;
   va_start ( ap, f );

   // Format the text.
   std::basic_string<Char> str;
   CadKit::format ( f, ap, str );

   // End variable argument processing.
   va_end ( ap );

   // Return the string;
   return str;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Split the string. If the delimiter is not found then the list should have 
//  one element, the given string.
//
///////////////////////////////////////////////////////////////////////////////

template<class Char, class String, class Sequence> inline bool split 
  ( const String &str, const Char &delimiter, Sequence &parts )
{
  // Note: do not clear the sequence. Let the client handle that because
  // he/she may want to call this function more than once with the same one.

  // Handle trivial case.
  if ( str.empty() )
    return false;

  // Needed below.
  String current;
  bool result = false;

  // Loop through the string.
  size_t size = str.length();
  for ( size_t i = 0; i < size; ++i )
  {
    // If this character is not the delimiter.
    if ( str[i] != delimiter )
    {
      // Append it to the current string.
      current += static_cast<Char>( str[i] );
    }

    // Otherwise...
    else
    {
      // Push the current string onto the list.
      parts.push_back ( current );

      // Clear the current string.
      current.erase();

      // Set the flag.
      result = true;
    }
  }

  // If we found the delimiter.
  if ( result )
  {
    // Push the last string onto the list.
    parts.push_back ( current );
  }

  // Return the result.
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assemble the string from the sequence. Pass 0 for the delimiter to not
//  have it inserted.
//
///////////////////////////////////////////////////////////////////////////////

template<class Char, class String, class Sequence> inline void assemble
  ( const Char &delimiter, const Sequence &parts, String &str )
{
  // Note: do not clear the string. Let the client handle that because he/she
  // may want to call this function more than once with the same string.

  // Loop through the list.
  unsigned int count = parts.size();
  Sequence::const_iterator i = parts.begin();
  for ( Sequence::const_iterator j = parts.begin(); j != parts.end(); ++j )
  {
    // Add the string.
    str += *j;

    // If this isn't the last one, and we are supposed to insert the delimiter...
    if ( ( 0 != count ) && delimiter )
    {
      // Add the delimiter.
      str += static_cast<Char>( delimiter );
    }

    // Decrement the count.
    --count;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Split the string.
//
///////////////////////////////////////////////////////////////////////////////

template<class Char, class String> inline bool splitAtFirst 
  ( const String &string, const Char &delimiter, String &left, String &right )
{
  // Initialize.
  left.clear();
  right.clear();

  // Split the string into a list.
  std::list<String> parts;
  if ( false == CadKit::split ( string, delimiter, parts ) )
    return false;

  // The first member of the list is the left.
  left = parts.front();

  // Remove the first element.
  parts.pop_front();

  // Assemble the rest of the list into the right portion.
  CadKit::assemble ( delimiter, parts, right );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Split the string.
//
///////////////////////////////////////////////////////////////////////////////

template<class Char, class String> inline bool splitAtLast 
  ( const String &str, const Char &delimiter, String &left, String &right )

{
  // Initialize.
  left.clear();
  right.clear();

  // Split the string into a list.
  std::list<String> parts;
  if ( false == CadKit::split ( str, delimiter, parts ) )
    return false;

  // The last member of the list is the right.
  right = parts.back();

  // Remove the last element.
  parts.pop_back();

  // Assemble the rest of the list into the left portion.
  CadKit::assemble ( delimiter, parts, left );

  // It worked.
  return true;
}


#ifdef _WIN32


///////////////////////////////////////////////////////////////////////////////
//
//  Set the string from the given error id.
//
///////////////////////////////////////////////////////////////////////////////

template<class F, class S, class B> inline bool setStringFromErrorId 
  ( const HRESULT &error, F function, S theString, B buffer )
{
  // The buffer to write to
  buffer = NULL;

  if ( function ( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  error,
                  MAKELANGID ( LANG_NEUTRAL, SUBLANG_DEFAULT ), // Default language.
                  (B) &buffer,
                  0,
                  NULL ) )
  {
    // If we have a string.
    if ( buffer )
    {
      // Set this string.
      theString == buffer;

      // Free the string.
      ::LocalFree ( buffer );

      // It worked.
      return true;
    }
  }

  // If we get to here then it didn't work (probably a non-standard error id).
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set this string from the given error id.
//
///////////////////////////////////////////////////////////////////////////////

inline bool setStringFromErrorId ( const HRESULT &error, std::basic_string<char> &str )
{
  // This just resolves the template function definition.
  char *buffer = NULL;

  // Call the base class's function.
  return CadKit::setStringFromErrorId ( error, ::FormatMessageA, str, buffer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set this string from the given error id.
//
///////////////////////////////////////////////////////////////////////////////

inline bool setStringFromErrorId ( const HRESULT &error, std::basic_string<wchar_t> &str )
{
  // This just resolves the template function definition.
  wchar_t *buffer = NULL;

  // Call the base class's function.
  return CadKit::setStringFromErrorId ( error, ::FormatMessageW, str, buffer );
}


#endif // _WIN32


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the string from ansi to wchar_t.
//
///////////////////////////////////////////////////////////////////////////////

inline void convert ( const std::basic_string<char> &s1, std::basic_string<wchar_t> &s2 )
{
  // Allocate space, calling "reserve()" doesn't work.
  s2.resize ( s1.size() );

  // Loop through the string.
  unsigned int size = s1.size();
  for ( unsigned int i = 0; i < size; ++i )
  {
    // Write the character.
    s2[i] = static_cast<wchar_t> ( s1[i] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the string from wchar_t to ansi.
//
///////////////////////////////////////////////////////////////////////////////

inline void convert ( const std::basic_string<wchar_t> &s1, std::basic_string<char> &s2 )
{
  // You may need to reverse the bit-shift below for a big-endian machine.
  // This is just a heads-up so you don't forget to check.
#ifdef _DEBUG
  unsigned long u = 1;
  SL_ASSERT ( 0 != ( *( (char *) &u ) ) );
#endif

  // Allocate space, calling "reserve()" doesn't work.
  s2.resize ( 2 * s1.size() );

  // Needed below.
  wchar_t wc;
  char c1, c2;
  unsigned int count = 0;

  // Loop through the string.
  unsigned int size = s1.size();
  for ( unsigned int i = 0; i < size; ++i )
  {
    // Grab the character.
    wc = s1[i];

    // If the character can fit in 8 bits...
    if ( wc < 256 )
    {
      // Cast to a char.
      s2[i] = static_cast<char> ( wc );
      ++count;
    }

    // Otherwise, we have to break it into two chars.
    else
    {
      // Shift the bits to get the two halves.
      c1 = static_cast<char> ( wc >> 8 );
      c2 = static_cast<char> ( wc );

      // Assign to the string.
      s2[i] = c1; ++i;
      s2[i] = c2;
      count += 2;
    }
  }

  // We may have extra stuff at the end because we allocated enough to hold 
  // two chars for each wchar_t. This will not erase the string, just truncate 
  // it at the end of the valid data.
  SL_ASSERT ( count <= s2.size() );
  s2.resize ( count );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the converted string.
//
///////////////////////////////////////////////////////////////////////////////

inline std::basic_string<wchar_t> convert ( const std::basic_string<char> &s )
{
  // Call the other one.
  std::basic_string<wchar_t> result;
  CadKit::convert ( s, result );
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the converted string.
//
///////////////////////////////////////////////////////////////////////////////

inline std::basic_string<char> convert ( const std::basic_string<wchar_t> &s )
{
  // Call the other one.
  std::basic_string<char> result;
  CadKit::convert ( s, result );
  return result;
}


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_STRING_FUNCTIONS_H_
