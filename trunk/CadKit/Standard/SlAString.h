
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
//  SlAString: Ascii-character string class.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _CADKIT_STANDARD_LIBRARY_ASCII_CHARACTER_STRING_CLASS_
#define _CADKIT_STANDARD_LIBRARY_ASCII_CHARACTER_STRING_CLASS_

#include "SlTString.h"
#include "SlFunctionDefines.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <stdio.h>
# include <stdarg.h>
#endif


namespace CadKit 
{
class SlAString : public SlTString<char>
{
public:

  // Constructor(s).
  explicit SlAString();
  SlAString ( const SlAString &copyMe );
  SlAString ( size_type pos, size_type n, const SlAString &copyMe );
  SlAString ( const std::basic_string<char> &copyMe );
  SlAString ( size_type pos, size_type n, const std::basic_string<char> &copyMe );
  SlAString ( const char *f, ... );

  // Append.
  SlAString &               append ( const char *s );
  SlAString &               append ( size_type n, const char *s );
  SlAString &               append ( size_type pos, size_type n, const SlAString &str );
  SlAString &               append ( const SlAString &str );
  SlAString &               append ( size_type pos, size_type n, const SlTString<char> &str );
  SlAString &               append ( const SlTString<char> &str );
  SlAString &               append ( size_type pos, size_type n, const std::basic_string<char> &str );
  SlAString &               append ( const std::basic_string<char> &str );
  SlAString &               append ( size_type n, char c );
  SlAString &               append ( const_iterator first, const_iterator last );
  SlAString &               append ( int i );
  SlAString &               append ( unsigned int i );
  SlAString &               append ( float d );
  SlAString &               append ( double d );

  // Format the string.
  void                      format ( const char *f, va_list ap );
  void                      format ( const char *f, ... );

  // Append operators.
  SlAString &               operator += ( const SlAString &rhs );
  SlAString &               operator += ( const SlTString<char> &rhs );
  SlAString &               operator += ( const std::basic_string<char> &rhs );
  SlAString &               operator += ( const char *s );
  SlAString &               operator += ( char c );
  SlAString &               operator += ( int i );
  SlAString &               operator += ( unsigned int i );
  SlAString &               operator += ( float d );
  SlAString &               operator += ( double d );

  // These will append.
  friend SlAString &        operator << ( SlAString &lhs, const SlAString &rhs );
  friend SlAString &        operator << ( SlAString &lhs, const SlTString<char> &rhs );
  friend SlAString &        operator << ( SlAString &lhs, const std::basic_string<char> &rhs );
  friend SlAString &        operator << ( SlAString &lhs, const char *rhs );
  friend SlAString &        operator << ( SlAString &lhs, char rhs );
  friend SlAString &        operator << ( SlAString &lhs, int i );
  friend SlAString &        operator << ( SlAString &lhs, unsigned int i );
  friend SlAString &        operator << ( SlAString &lhs, float d );
  friend SlAString &        operator << ( SlAString &lhs, double d );

  // Set this string from the given error id.
  #ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_SET_FROM_ERROR_ID
  bool                      setFromErrorId ( const HRESULT &error );
  #endif

  void                      split ( const char &delimiter, std::list<SlAString> &components ) const;
  
  // Convert all characters to upper/lower case.
  void                      toLower();
  void                      toUpper();
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

inline SlAString::SlAString() : SlTString<char>(){}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

inline SlAString::SlAString ( const SlAString &rhs ) : 
  SlTString<char> ( rhs ){}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

inline SlAString::SlAString ( size_type pos, 
                              size_type n,
                              const SlAString &copyMe ) : 
   SlTString<char> ( pos, n, copyMe ){}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

inline SlAString::SlAString ( const std::basic_string<char> &copyMe ) : 
  SlTString<char> ( copyMe ){}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

inline SlAString::SlAString ( size_type pos, 
                              size_type n,
                              const std::basic_string<char> &copyMe ) : 
  SlTString<char> ( pos, n, copyMe ){}


///////////////////////////////////////////////////////////////////////////////
//
// Format the string.
//
///////////////////////////////////////////////////////////////////////////////

inline void SlAString::format ( const char *f, va_list ap )
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
   this->setValue ( buf );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

inline SlAString::SlAString ( const char *f, ... ) : SlTString<char>()
{
  // If we have a valid format string.
  if ( f )
  {
    // Start variable argument processing.
    va_list ap;
    va_start ( ap, f );

    // Format the text.
    this->format ( f, ap );

    // End variable argument processing.
    va_end ( ap );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Format the string.
//
///////////////////////////////////////////////////////////////////////////////

inline void SlAString::format ( const char *f, ... )
{
   SL_ASSERT ( f );

   // Start variable argument processing.
   va_list ap;
   va_start ( ap, f );

   // Format the text.
   this->format ( f, ap );

   // End variable argument processing.
   va_end ( ap );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlAString &SlAString::append ( const SlAString &s )
{
  SlTString<char>::append ( s );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlAString &SlAString::append ( const char *s )
{
  SlTString<char>::append ( s );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlAString &SlAString::append ( size_type n, const char *s )
{
  SlTString<char>::append ( n, s );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlAString &SlAString::append ( size_type pos, 
                                      size_type n,
                                      const SlTString<char> &str )
{
  SlTString<char>::append ( pos, n, str );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlAString &SlAString::append ( const SlTString<char> &str )
{
  SlTString<char>::append ( str );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlAString &SlAString::append ( size_type pos, 
                                      size_type n,
                                      const std::basic_string<char> &str )
{
  SlTString<char>::append ( pos, n, str );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlAString &SlAString::append ( const std::basic_string<char> &str )
{
  SlTString<char>::append ( str );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlAString &SlAString::append ( size_type n, char c )
{
  SlTString<char>::append ( n, c );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlAString &SlAString::append ( const_iterator first, const_iterator last )
{
  SlTString<char>::append ( first, last );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append functions.
//
///////////////////////////////////////////////////////////////////////////////

SL_TEMPLATE_STRING_APPEND_FUNCTION ( SlAString, char, int,          "%d", SL_SNPRINTF, SL_STRING_FUNCTION_BUFFER_SIZE );
SL_TEMPLATE_STRING_APPEND_FUNCTION ( SlAString, char, unsigned int, "%u", SL_SNPRINTF, SL_STRING_FUNCTION_BUFFER_SIZE );
SL_TEMPLATE_STRING_APPEND_FUNCTION ( SlAString, char, float,        "%f", SL_SNPRINTF, SL_STRING_FUNCTION_BUFFER_SIZE );
SL_TEMPLATE_STRING_APPEND_FUNCTION ( SlAString, char, double,       "%f", SL_SNPRINTF, SL_STRING_FUNCTION_BUFFER_SIZE );


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the string to upper case.
//
///////////////////////////////////////////////////////////////////////////////

inline void SlAString::toUpper()
{
  size_type length = this->length();
  for ( size_type i = 0; i < length; ++i )
    (*this)[i] = (char) ::toupper ( (*this)[i] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the string to lower case.
//
///////////////////////////////////////////////////////////////////////////////

inline void SlAString::toLower()
{
  size_type length = this->length();
  for ( size_type i = 0; i < length; ++i )
    (*this)[i] = (char) ::tolower ( (*this)[i] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  operator +=.
//
///////////////////////////////////////////////////////////////////////////////

SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlAString, const SlAString & );
SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlAString, const SlTString<char> & );
SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlAString, const std::basic_string<char> & );
SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlAString, const char * );
SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlAString, char );
SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlAString, int );
SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlAString, unsigned int );
SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlAString, float );
SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlAString, double );


///////////////////////////////////////////////////////////////////////////////
//
//  operator <<.
//
///////////////////////////////////////////////////////////////////////////////

SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlAString, const SlAString & );
SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlAString, const SlTString<char> & );
SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlAString, const std::basic_string<char> & );
SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlAString, const char * );
SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlAString, char );
SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlAString, int );
SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlAString, unsigned int );
SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlAString, float );
SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlAString, double );


///////////////////////////////////////////////////////////////////////////////
//
//  Split the string.
//
///////////////////////////////////////////////////////////////////////////////

inline void SlAString::split ( const char &delimiter, std::list<SlAString> &components ) const
{
  // Call the non-member function.
  CadKit::_splitStringIntoList ( *this, delimiter, components );
}


#ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_SET_FROM_ERROR_ID


///////////////////////////////////////////////////////////////////////////////
//
//  Set this string from the given error id.
//
///////////////////////////////////////////////////////////////////////////////

inline bool SlAString::setFromErrorId ( const HRESULT &error )
{
  // This just resolves the template function definition.
  char *buffer = NULL;

  // Call the base class's function.
  return CadKit::_setFromErrorId ( error, ::FormatMessageA, *this, buffer );
}


#endif // CADKIT_DEFINE_SL_TEMPLATE_STRING_SET_FROM_ERROR_ID


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_ASCII_CHARACTER_STRING_CLASS_
