
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
//  SlWString: Wide-character string class.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _CADKIT_STANDARD_LIBRARY_WIDE_CHARACTER_STRING_CLASS_
#define _CADKIT_STANDARD_LIBRARY_WIDE_CHARACTER_STRING_CLASS_

#include "SlTString.h"
#include "SlFunctionDefines.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <stdio.h>
# include <stdarg.h>
#endif


namespace CadKit 
{
class SlWString : public SlTString<wchar_t>
{
public:

  // Constructor(s).
  explicit SlWString();
  SlWString ( const SlWString &copyMe );
  SlWString ( size_type pos, size_type n, const SlWString &copyMe );
  SlWString ( const std::basic_string<wchar_t> &copyMe );
  SlWString ( size_type pos, size_type n, const std::basic_string<wchar_t> &copyMe );
  SlWString ( const wchar_t *f, ... );

  // Append.
  SlWString &               append ( const wchar_t *s );
  SlWString &               append ( size_type n, const wchar_t *s );
  SlWString &               append ( size_type pos, size_type n, const SlWString &str );
  SlWString &               append ( const SlWString &str );
  SlWString &               append ( size_type pos, size_type n, const SlTString<wchar_t> &str );
  SlWString &               append ( const SlTString<wchar_t> &str );
  SlWString &               append ( size_type pos, size_type n, const std::basic_string<wchar_t> &str );
  SlWString &               append ( const std::basic_string<wchar_t> &str );
  SlWString &               append ( size_type n, wchar_t c );
  SlWString &               append ( const_iterator first, const_iterator last );
  SlWString &               append ( int i );
  SlWString &               append ( unsigned int i );
  SlWString &               append ( float d );
  SlWString &               append ( double d );

  // Format the string.
  void                      format ( const wchar_t *f, va_list ap );
  void                      format ( const wchar_t *f, ... );

  // Append operators.
  SlWString &               operator += ( const SlWString &rhs );
  SlWString &               operator += ( const SlTString<wchar_t> &rhs );
  SlWString &               operator += ( const std::basic_string<wchar_t> &rhs );
  SlWString &               operator += ( const wchar_t *s );
  SlWString &               operator += ( wchar_t c );
  SlWString &               operator += ( int i );
  SlWString &               operator += ( unsigned int i );
  SlWString &               operator += ( float d );
  SlWString &               operator += ( double d );

  // These will append.
  friend SlWString &        operator << ( SlWString &lhs, const SlWString &rhs );
  friend SlWString &        operator << ( SlWString &lhs, const SlTString<wchar_t> &rhs );
  friend SlWString &        operator << ( SlWString &lhs, const std::basic_string<wchar_t> &rhs );
  friend SlWString &        operator << ( SlWString &lhs, const wchar_t *rhs );
  friend SlWString &        operator << ( SlWString &lhs, wchar_t rhs );
  friend SlWString &        operator << ( SlWString &lhs, int i );
  friend SlWString &        operator << ( SlWString &lhs, unsigned int i );
  friend SlWString &        operator << ( SlWString &lhs, float d );
  friend SlWString &        operator << ( SlWString &lhs, double d );

  // Set this string from the given error id.
  #ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_SET_FROM_ERROR_ID
  bool                      setFromErrorId ( const HRESULT &error );
  #endif

  #ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_SPLIT_INTO_LIST_FUNCTION
  void                      split ( const wchar_t &delimiter, std::list<SlWString> &components ) const;
  #endif
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString::SlWString() : SlTString<wchar_t>(){}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString::SlWString ( const SlWString &copyMe ) : 
  SlTString<wchar_t> ( copyMe ){}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString::SlWString ( size_type pos, 
                              size_type n,
                              const SlWString &copyMe ) : 
  SlTString<wchar_t> ( pos, n, copyMe ){}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString::SlWString ( const std::basic_string<wchar_t> &copyMe ) : 
  SlTString<wchar_t> ( copyMe ){}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString::SlWString ( size_type pos, 
                              size_type n,
                              const std::basic_string<wchar_t> &copyMe ) : 
  SlTString<wchar_t> ( pos, n, copyMe ){}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString::SlWString ( const wchar_t *f, ... ) : SlTString<wchar_t>()
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

inline void SlWString::format ( const wchar_t *f, ... )
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
// Format the string.
//
///////////////////////////////////////////////////////////////////////////////

inline void SlWString::format ( const wchar_t *f, va_list ap )
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
   this->setValue ( buf );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString &SlWString::append ( const wchar_t *s )
{
  SlTString<wchar_t>::append ( s );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString &SlWString::append ( size_type n, const wchar_t *s )
{
  SlTString<wchar_t>::append ( n, s );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString &SlWString::append ( size_type pos, size_type n, const SlWString &str )
{
  SlTString<wchar_t>::append ( pos, n, str );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString &SlWString::append ( const SlWString &str )
{
  SlTString<wchar_t>::append ( str );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString &SlWString::append ( size_type pos, 
                                      size_type n,
                                      const SlTString<wchar_t> &str )
{
  SlTString<wchar_t>::append ( pos, n, str );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString &SlWString::append ( const SlTString<wchar_t> &str )
{
  SlTString<wchar_t>::append ( str );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString &SlWString::append ( size_type pos, 
                                      size_type n,
                                      const std::basic_string<wchar_t> &str )
{
  SlTString<wchar_t>::append ( pos, n, str );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString &SlWString::append ( const std::basic_string<wchar_t> &str )
{
  SlTString<wchar_t>::append ( str );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString &SlWString::append ( size_type n, wchar_t c )
{
  SlTString<wchar_t>::append ( n, c );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString &SlWString::append ( const_iterator first, const_iterator last )
{
  SlTString<wchar_t>::append ( first, last );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append functions.
//
///////////////////////////////////////////////////////////////////////////////

SL_TEMPLATE_STRING_APPEND_FUNCTION ( SlWString, wchar_t, int,           L"%d", ::_snwprintf, SL_STRING_FUNCTION_BUFFER_SIZE );
SL_TEMPLATE_STRING_APPEND_FUNCTION ( SlWString, wchar_t, unsigned int,  L"%u", ::_snwprintf, SL_STRING_FUNCTION_BUFFER_SIZE );
SL_TEMPLATE_STRING_APPEND_FUNCTION ( SlWString, wchar_t, float,         L"%f", ::_snwprintf, SL_STRING_FUNCTION_BUFFER_SIZE );
SL_TEMPLATE_STRING_APPEND_FUNCTION ( SlWString, wchar_t, double,        L"%f", ::_snwprintf, SL_STRING_FUNCTION_BUFFER_SIZE );


///////////////////////////////////////////////////////////////////////////////
//
//  operator +=.
//
///////////////////////////////////////////////////////////////////////////////

SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlWString, const SlWString & );
SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlWString, const SlTString<wchar_t> & );
SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlWString, const std::basic_string<wchar_t> & );
SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlWString, const wchar_t * );
SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlWString, wchar_t );
SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlWString, int );
SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlWString, unsigned int );
SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlWString, float );
SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR ( SlWString, double );


///////////////////////////////////////////////////////////////////////////////
//
//  operator <<.
//
///////////////////////////////////////////////////////////////////////////////

SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlWString, const SlWString & );
SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlWString, const SlTString<wchar_t> & );
SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlWString, const std::basic_string<wchar_t> & );
SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlWString, const wchar_t * );
SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlWString, wchar_t );
SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlWString, int );
SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlWString, unsigned int );
SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlWString, float );
SL_TEMPLATE_STRING_OUTPUT_OPERATOR ( SlWString, double );


#ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_SPLIT_INTO_LIST_FUNCTION


///////////////////////////////////////////////////////////////////////////////
//
//  Split the string.
//
///////////////////////////////////////////////////////////////////////////////

inline void SlWString::split ( const wchar_t &delimiter, std::list<SlWString> &components ) const
{
  // Call the non-member function.
  CadKit::_splitStringIntoList ( *this, delimiter, components );
}


#endif // CADKIT_DEFINE_SL_TEMPLATE_STRING_SPLIT_INTO_LIST_FUNCTION


#ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_SET_FROM_ERROR_ID


///////////////////////////////////////////////////////////////////////////////
//
//  Set this string from the given error id.
//
///////////////////////////////////////////////////////////////////////////////

inline bool SlWString::setFromErrorId ( const HRESULT &error )
{
  // This just resolves the template function definition.
  wchar_t *buffer = NULL;

  // Call the base class's function.
  return CadKit::_setFromErrorId ( error, ::FormatMessageW, *this, buffer );
}


#endif // CADKIT_DEFINE_SL_TEMPLATE_STRING_SET_FROM_ERROR_ID


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_WIDE_CHARACTER_STRING_CLASS_
