
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
//  SlTString: A template string that inherits from std::basic_string.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _CADKIT_STANDARD_LIBRARY_TEMPLATE_STRING_CLASS_
#define _CADKIT_STANDARD_LIBRARY_TEMPLATE_STRING_CLASS_

#include "SlTypedefs.h"
#include "SlAssert.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <stdarg.h>
# include <string>
# if __GNUC__
   typedef basic_string <wchar_t> wstring; // See /usr/include/g++-3/string
# endif
# include <algorithm>
# ifdef _WIN32
#  include <locale>
# endif
#endif

#ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_OSTREAM_OPERATOR
# include <iostream.h>
#endif

#ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_STD_OSTREAM_OPERATOR
# include <iostream>
#endif

#ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_ISTREAM_OPERATOR
# include <iostream.h>
#endif

#ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_STD_ISTREAM_OPERATOR
# include <iostream>
#endif

#ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_SPLIT_INTO_LIST_FUNCTION
# include <list>
#endif

#ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_SET_FROM_ERROR_ID
# ifdef _WIN32
#  include "windows.h"
# else  // Unix
TODO
# endif // Win32/Unix.
#endif


namespace CadKit
{
template<class T> class SlTString : public std::basic_string<T>
{
public:

  // Constructors.
  explicit SlTString();
  SlTString ( const SlTString &copyMe );
  SlTString ( size_type pos, size_type n, const SlTString &copyMe );
  SlTString ( const std::basic_string<T> &copyMe );
  SlTString ( size_type pos, size_type n, const std::basic_string<T> &copyMe );
  SlTString ( const T *s );

  // Append.
  SlTString &               append ( size_type pos, size_type n, const SlTString<T> &str );
  SlTString &               append ( const SlTString<T> &str );
  SlTString &               append ( size_type pos, size_type n, const std::basic_string<T> &str );
  SlTString &               append ( const std::basic_string<T> &str );
  SlTString &               append ( const T *s );
  SlTString &               append ( size_type n, const T *s );
  SlTString &               append ( size_type n, T c );
  SlTString &               append ( const_iterator first, const_iterator last );

  // Clear the string. Some implementations of std::basic_string do not have this.
  void                      clear();

  // Return the first and last letters, or '\0' if the string is empty.
  T                         getFirst() const { return ( this->empty() ) ? ((T)0) : this->at ( 0 ); }
  T                         getLast()  const { return ( this->empty() ) ? ((T)0) : this->at ( this->size() - 1 ); }

  // See if the strings are equal.
  bool                      isEqual ( const SlTString<T> &str, const bool &caseInsensitive ) const;
  bool                      isEqual ( const std::basic_string<T> &str, const bool &caseInsensitive ) const;
  bool                      isEqual ( const T *str, const bool &caseInsensitive ) const;
  bool                      isEqual ( const SlTString<T> &str ) const;
  bool                      isEqual ( const std::basic_string<T> &str ) const;
  bool                      isEqual ( const T *str ) const;

  // On Windows this does a case-insensitive comparison. On unix it does 
  // a case-sensitive comparison.
  bool                      isSameFile ( const SlTString<T> &str ) const;
  bool                      isSameFile ( const std::basic_string<T> &str ) const;
  bool                      isSameFile ( const T *str ) const;

  // Typecast operators.
  operator                  const T * const () const { return this->c_str(); }
#ifdef _WIN32
  operator                  const std::basic_string<T> & () const { return *this; }
  operator                  std::basic_string<T> & () { return *this; }
#endif

  // Bracket operator.
  const T &                 operator [] ( size_t index ) const;
  T &                       operator [] ( size_t index );

  // Assignment operators.
  SlTString &               operator = ( const SlTString &rhs );
  SlTString &               operator = ( const std::basic_string<T> &rhs );
  SlTString &               operator = ( const T *s );
  SlTString &               operator = ( T c );

  // Append operators.
  SlTString &               operator += ( const SlTString &rhs );
  SlTString &               operator += ( const std::basic_string<T> &rhs );
  SlTString &               operator += ( const T *s );
  SlTString &               operator += ( T c );

  // These will append.
#if __GNUC__ >= 2
  template<class P> friend SlTString<P> &operator << ( SlTString<P> &lhs, const SlTString<P> &rhs );
  template<class P> friend SlTString<P> &operator << ( SlTString<P> &lhs, const std::basic_string<P> &rhs );
  template<class P> friend SlTString<P> &operator << ( SlTString<P> &lhs, const P *rhs );
  template<class P> friend SlTString<P> &operator << ( SlTString<P> &lhs, P rhs );
#else
  friend SlTString &        operator << ( SlTString &lhs, const SlTString &rhs );
  friend SlTString &        operator << ( SlTString &lhs, const std::basic_string<T> &rhs );
  friend SlTString &        operator << ( SlTString &lhs, const T *rhs );
  friend SlTString &        operator << ( SlTString &lhs, T rhs );
#endif

  // Equality.
#if __GNUC__ >= 2
  template<class P> friend bool operator == ( const SlTString<P> &sa, const SlTString<P> &sb );
  template<class P> friend bool operator == ( const SlTString<P> &sa, const std::basic_string<P> &sb );
  template<class P> friend bool operator == ( const SlTString<P> &sa, const P *sb );
  template<class P> friend bool operator == ( const SlTString<P> &sa, P sb );
  template<class P> friend bool operator == ( const std::basic_string<P> &sa, const SlTString<P> &sb );
  template<class P> friend bool operator == ( const P *sa, const SlTString<P> &sb );
  template<class P> friend bool operator == ( P sa, const SlTString<P> &sb );
#else
  friend bool               operator == ( const SlTString &sa, const SlTString &sb );
  friend bool               operator == ( const SlTString &sa, const std::basic_string<T> &sb );
  friend bool               operator == ( const SlTString &sa, const T *sb );
  friend bool               operator == ( const SlTString &sa, T sb );
  friend bool               operator == ( const std::basic_string<T> &sa, const SlTString &sb );
  friend bool               operator == ( const T *sa, const SlTString &sb );
  friend bool               operator == ( T sa, const SlTString &sb );
#endif

  // Inequality.
#if __GNUC__ >= 2
  template<class P> friend bool operator != ( const SlTString<P> &sa, const SlTString<P> &sb );
  template<class P> friend bool operator != ( const SlTString<P> &sa, const std::basic_string<P> &sb );
  template<class P> friend bool operator != ( const SlTString<P> &sa, const P *sb );
  template<class P> friend bool operator != ( const SlTString<P> &sa, P sb );
  template<class P> friend bool operator != ( const std::basic_string<P> &sa, const SlTString<P> &sb );
  template<class P> friend bool operator != ( const P *sa, const SlTString<P> &sb );
  template<class P> friend bool operator != ( P sa, const SlTString<P> &sb );
#else
  friend bool               operator != ( const SlTString &sa, const SlTString &sb );
  friend bool               operator != ( const SlTString &sa, const std::basic_string<T> &sb );
  friend bool               operator != ( const SlTString &sa, const T *sb );
  friend bool               operator != ( const SlTString &sa, T sb );
  friend bool               operator != ( const std::basic_string<T> &sa, const SlTString &sb );
  friend bool               operator != ( const T *sa, const SlTString &sb );
  friend bool               operator != ( T sa, const SlTString &sb );
#endif

  // I/O.
#ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_OSTREAM_OPERATOR
#if __GNUC__ >= 2
  template<class P> friend ::ostream &operator << ( ::ostream &out, const SlTString<P> &s );
#else
  friend ::ostream &        operator << ( ::ostream &out, const SlTString &s );
#endif
#endif

#ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_STD_OSTREAM_OPERATOR
#if __GNUC__ >= 2
  template<class P> friend std::ostream &operator << ( std::ostream &out, const SlTString<P> &s );
#else
  friend std::ostream &     operator << ( std::ostream &out, const SlTString &s );
#endif
#endif

#ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_ISTREAM_OPERATOR
#if __GNUC__ >= 2
  template<class P> friend ::istream &operator >> ( ::istream &in, SlTString<P> &s );
#else
  friend ::istream &        operator >> ( ::istream &in, SlTString &s );
#endif
#endif

#ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_STD_ISTREAM_OPERATOR
#if __GNUC__ >= 2
  template<class P> friend std::istream &operator >> ( std::istream &in, SlTString<P> &s );
#else
  friend std::istream &     operator >> ( std::istream &in, SlTString &s );
#endif
#endif

  // Replace all occurances of the one character with the other.
  void                      replace ( const T &oldChar, const T &newChar );

  // Reverse the order of the string.
  void                      reverse();

  // Set the value.
  void                      setValue ( const SlTString &rhs );
  void                      setValue ( size_type pos, size_type n, const SlTString &str );
  void                      setValue ( const std::basic_string<T> &rhs );
  void                      setValue ( size_type pos, size_type n, const std::basic_string<T> &str );
  void                      setValue ( size_type n, const T *s );
  void                      setValue ( const T *s );
  void                      setValue ( size_type n, T c );

  // Convert all characters to upper/lower case. Pass true for "useLocale" to 
  // use the location information. Pass false to treat the characters as if 
  // they were English. Not sure what that means for unicode.
  void                      toLower ( const bool &useLocale );
  void                      toUpper ( const bool &useLocale );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T>::SlTString() : std::basic_string<T>(){}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T>::SlTString ( const SlTString &rhs ) : 
  std::basic_string<T> ( rhs ){}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T>::SlTString ( size_type pos, 
                                                   size_type n,
                                                   const SlTString &copyMe ) : 
  std::basic_string<T> ( copyMe, pos, n ){}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T>::SlTString ( const std::basic_string<T> &rhs ) : 
  std::basic_string<T> ( rhs ){}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T>::SlTString ( size_type pos, 
                                                   size_type n,
                                                   const std::basic_string<T> &str ) : 
  std::basic_string<T> ( str, pos, n ){}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T>::SlTString ( const T *s ) : 
  std::basic_string<T> ( s ){}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &SlTString<T>::append ( const SlTString<T> &s )
{
  ((std::basic_string<T> &) *this).append ( static_cast < std::basic_string<T> >(s) );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &SlTString<T>::append ( const T *s )
{
  ((std::basic_string<T> &) *this).append ( s );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &SlTString<T>::append ( size_type n, const T *s )
{
  ((std::basic_string<T> &) *this).append ( s, n );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &SlTString<T>::append ( size_type pos, 
                                                              size_type n,
                                                              const std::basic_string<T> &str )
{
  ((std::basic_string<T> &) *this).append ( str, pos, n );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &SlTString<T>::append ( const std::basic_string<T> &str )
{
  ((std::basic_string<T> &) *this).append ( str );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &SlTString<T>::append ( size_type n, T c )
{
  ((std::basic_string<T> &) *this).append ( n, c );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &SlTString<T>::append ( const_iterator first, 
                                                              const_iterator last )
{
  ((std::basic_string<T> &) *this).append ( first, last );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the string. Some implementations of std::basic_string do not have.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlTString<T>::clear()
{
  // Removes from [first,last), so we can pass "end()" for "last".
  this->erase ( this->begin(), this->end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  On Windows this does a case-insensitive comparison. On unix it does 
//  a case-sensitive comparison.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlTString<T>::isSameFile ( const SlTString<T> &str ) const
{
#ifdef _WIN32
  return this->isEqual ( str, true );
#else
  return this->isEqual ( str, false );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  On Windows this does a case-insensitive comparison. On unix it does 
//  a case-sensitive comparison.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlTString<T>::isSameFile ( const std::basic_string<T> &str ) const
{
#ifdef _WIN32
  return this->isEqual ( str, true );
#else
  return this->isEqual ( str, false );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  On Windows this does a case-insensitive comparison. On unix it does 
//  a case-sensitive comparison.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlTString<T>::isSameFile ( const T *str ) const
{
#ifdef _WIN32
  return this->isEqual ( str, true );
#else
  return this->isEqual ( str, false );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given string is equal to this string.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlTString<T>::isEqual ( const SlTString<T> &str, 
                                                      const bool &caseInsensitive ) const
{
  // If we are doing case-insensitive...
  if ( caseInsensitive )
  {
    // Make copies and convert them to upper case.
    SlTString<T> sa ( *this ), sb ( str );
    sa.toUpper ( true );
    sb.toUpper ( true );

    // Compare the upper-case strings.
    return sa.isEqual ( sb );
  }

  // Otherwise, compare them as-is.
  return this->isEqual ( str );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given string is equal to this string.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlTString<T>::isEqual ( const std::basic_string<T> &str, 
                                                      const bool &caseInsensitive ) const
{
  // If we are doing case-insensitive...
  if ( caseInsensitive )
  {
    // Make copies and convert them to upper case.
    SlTString<T> sa ( *this ), sb ( str );
    sa.toUpper ( true );
    sb.toUpper ( true );

    // Compare the upper-case strings.
    return sa.isEqual ( sb );
  }

  // Otherwise, compare them as-is.
  return this->isEqual ( str );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given string is equal to this string.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlTString<T>::isEqual ( const T *str, 
                                                      const bool &caseInsensitive ) const
{
  // If we are doing case-insensitive...
  if ( caseInsensitive )
  {
    // Make copies and convert them to upper case.
    SlTString<T> sa ( *this ), sb ( str );
    sa.toUpper ( true );
    sb.toUpper ( true );

    // Compare the upper-case strings.
    return sa.isEqual ( sb );
  }

  // Otherwise, compare them as-is.
  return this->isEqual ( str );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given string is equal to this string.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlTString<T>::isEqual ( const SlTString<T> &str ) const
{
  return ( 0 == this->compare ( str ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given string is equal to this string.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlTString<T>::isEqual ( const std::basic_string<T> &str ) const
{
  return ( 0 == this->compare ( str ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given string is equal to this string.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlTString<T>::isEqual ( const T *str ) const
{
  return ( 0 == this->compare ( str ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bracket operator. For some reason the compiler doesn't like it when the 
//  integer is passed by "const size_t &".
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline const T &SlTString<T>::operator [] ( size_t index ) const
{
  SL_ASSERT ( this && index >= 0 && index < this->size() );
  return this->at ( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bracket operator. For some reason the compiler doesn't like it when the 
//  integer is passed by "const size_t &".
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T &SlTString<T>::operator [] ( size_t index )
{
  SL_ASSERT ( this && index >= 0 && index < this->size() );
  return this->at ( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &SlTString<T>::operator = ( const SlTString<T> &rhs )
{
  ((std::basic_string<T> &) *this).operator = ( static_cast < std::basic_string<T> >(rhs) );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &SlTString<T>::operator = ( const std::basic_string<T> &rhs )
{
  ((std::basic_string<T> &) *this).operator = ( rhs );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &SlTString<T>::operator = ( const T *s )
{
  ((std::basic_string<T> &) *this).operator = ( s );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &SlTString<T>::operator = ( T c )
{
  ((std::basic_string<T> &) *this).operator = ( c );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &SlTString<T>::operator += ( const SlTString<T> &rhs )
{
  ((std::basic_string<T> &) *this).operator += ( static_cast < std::basic_string<T> >(rhs) );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &SlTString<T>::operator += ( const std::basic_string<T> &rhs )
{
  ((std::basic_string<T> &) *this).operator += ( rhs );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &SlTString<T>::operator += ( const T *s )
{
  ((std::basic_string<T> &) *this).operator += ( s );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &SlTString<T>::operator += ( T c )
{
  ((std::basic_string<T> &) *this).operator += ( c );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlTString<T>::setValue ( const SlTString<T> &rhs )
{
  ((std::basic_string<T> &) *this).operator = ( static_cast < std::basic_string<T> >(rhs) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlTString<T>::setValue ( size_type pos, 
                                                       size_type n,
                                                       const SlTString<T> &str )
{
  ((std::basic_string<T> &) *this).operator = ( std::basic_string<T> ( rhs, pos, n ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlTString<T>::setValue ( const std::basic_string<T> &rhs )
{
  ((std::basic_string<T> &) *this).operator = ( rhs );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlTString<T>::setValue ( size_type pos, 
                                                       size_type n,
                                                       const std::basic_string<T> &str )
{
  ((std::basic_string<T> &) *this).operator = ( std::basic_string<T> ( rhs, pos, n ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlTString<T>::setValue ( size_type n, const T *s )
{
  ((std::basic_string<T> &) *this).operator = ( std::basic_string<T> ( s, n ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlTString<T>::setValue ( const T *s )
{
  ((std::basic_string<T> &) *this).operator = ( std::basic_string<T> ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlTString<T>::setValue ( size_type n, T c )
{
  ((std::basic_string<T> &) *this).operator = ( std::basic_string<T> ( n, c ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Replace all occurances of the one character with the other.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlTString<T>::replace ( const T &oldChar, 
                                                      const T &newChar )
{
  // Loop through all the characters.
  size_t length = this->size();
  for ( size_t i = 0; i < length; ++i )
  {
    // If the current character is the right one then replace it.
    if ( oldChar == (*this)[i] ) (*this)[i] = newChar;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reverse the order of the string.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlTString<T>::reverse()
{
  std::reverse ( this->begin(), this->end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &operator << ( SlTString<T> &lhs, 
                                                     const SlTString<T> &s )
{
  lhs.append ( s );
  return lhs;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &operator << ( SlTString<T> &lhs, 
                                                     const std::basic_string<T> &s )
{
  lhs.append ( s );
  return lhs;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &operator << ( SlTString<T> &lhs, 
                                                     const T *s )
{
  lhs.append ( s );
  return lhs;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlTString<T> &operator << ( SlTString<T> &lhs, T c )
{
  lhs.append ( c );
  return lhs;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator == ( const SlTString<T> &sa, const SlTString<T> &sb )
{
  return sa.isEqual ( sb );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator == ( const SlTString<T> &sa, const std::basic_string<T> &sb )
{
  return sa.isEqual ( sb );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator == ( const std::basic_string<T> &sa, const SlTString<T> &sb )
{
  return sb.isEqual ( sa );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator == ( const SlTString<T> &sa, const T *sb )
{
  return sa.isEqual ( sb );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator == ( const SlTString<T> &sa, T sb )
{
  return ( 0 == sa.compare ( SlTString<T> ( sb ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator == ( const T *sa, const SlTString<T> &sb )
{
  return ( 0 == sb.compare ( sa ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator == ( T sa, const SlTString<T> &sb )
{
  return ( 0 == sb.compare ( SlTString<T> ( sa ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator != ( const SlTString<T> &sa, const SlTString<T> &sb )
{
  return ( 0 != sa.compare ( sb ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator != ( const SlTString<T> &sa, const std::basic_string<T> &sb )
{
  return ( 0 != sa.compare ( sb ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator != ( const std::basic_string<T> &sa, const SlTString<T> &sb )
{
  return ( 0 != sa.compare ( sb ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator != ( const SlTString<T> &sa, const T *sb )
{
  return ( 0 != sa.compare ( sb ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator != ( const SlTString<T> &sa, T sb )
{
  return ( 0 != sa.compare ( SlTString<T> ( sb ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator != ( const T *sa, const SlTString<T> &sb )
{
  return ( 0 != sb.compare ( sa ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator != ( T sa, const SlTString<T> &sb )
{
  return ( 0 != sb.compare ( SlTString<T> ( sa ) ) );
}


#ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_SPLIT_INTO_LIST_FUNCTION


///////////////////////////////////////////////////////////////////////////////
//
//  Split the string. This is a template function (not a member).
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class L> inline void _splitStringIntoList ( const SlTString<T> &string, 
                                                              const T &delimiter, 
                                                              std::list<L> &components )
{
  // Clear the list.
  components.clear();

  // Handle trivial case.
  if ( string.empty() ) return;

  // Needed below.
  SlTString<T> current;

  // Loop through the string.
  size_t size = string.length();
  for ( size_t i = 0; i < size; ++i )
  {
    // If this character is not the delimiter, or if it's the last one...
    if ( string.at ( i ) != delimiter )
    {
      // Append it to the current string.
      current += string.at ( i );
    }

    // Otherwise...
    else
    {
      // Push the current string onto the list.
      components.push_back ( current );

      // Clear the current string.
      current.erase();
    }
  }

  // Push the last string onto the list.
  components.push_back ( current );
}


#endif // CADKIT_DEFINE_SL_TEMPLATE_STRING_SPLIT_INTO_LIST_FUNCTION


///////////////////////////////////////////////////////////////////////////////
//
//  These classes are used in the toUpper() and toLower() member functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

template<class T> class ToUpper
{
public:
  ToUpper() : _loc ( "C" ){}
  T operator() ( T c ) { return std::toupper ( c, _loc ); }
private:
  std::locale const _loc;
};

template<class T> class ToLower
{
public:
  ToLower() : _loc ( "C" ){}
  T operator() ( T c ) { return std::tolower ( c, _loc ); }
private:
  std::locale const _loc;
};

#endif // _WIN32


///////////////////////////////////////////////////////////////////////////////
//
//  Make the string all upper case.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlTString<T>::toUpper ( const bool &useLocale )
{
  // The "right" way (the one that considers locales) is at
  // http://gcc.gnu.org/onlinedocs/libstdc++/22_locale/howto.html
  if ( useLocale )
  {
#ifdef _WIN32
    ToUpper<T> up;
    std::transform ( this->begin(), this->end(), this->begin(), up );
    return;
#else // Not _WIN32
    SL_ASSERT ( 0 ); // TODO. For now, continue and do English-only way.
#endif // _WIN32
  }

  // The simple English-only way is at
  // http://sunsite.ualberta.ca/Documentation/Gnu/libstdc++-2.90.8/html/21_strings/howto.html#4
  std::transform ( this->begin(), this->end(), this->begin(), ::toupper );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the string all lower case.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlTString<T>::toLower ( const bool &useLocale )
{
  // The "right" way (the one that considers locales) is at
  // http://gcc.gnu.org/onlinedocs/libstdc++/22_locale/howto.html
  if ( useLocale )
  {
#ifdef _WIN32
    ToLower<T> down;
    std::transform ( this->begin(), this->end(), this->begin(), down );
    return;
#else // Not _WIN32
    SL_ASSERT ( 0 ); // TODO. For now, continue and do English-only way.
#endif // _WIN32
  }

  // The simple English-only way is at
  // http://sunsite.ualberta.ca/Documentation/Gnu/libstdc++-2.90.8/html/21_strings/howto.html#4
  std::transform ( this->begin(), this->end(), this->begin(), ::tolower );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro used in derived classes to implement the append() members.
//
///////////////////////////////////////////////////////////////////////////////

#define SL_TEMPLATE_STRING_APPEND_FUNCTION(class_name,buffer_type,data_type,format_string,format_function,buffer_size) \
inline class_name &class_name::append ( data_type data ) \
{ \
  buffer_type buffer[buffer_size]; \
  SL_VERIFY ( -1 != format_function ( buffer, buffer_size, format_string, data ) ); \
  SlTString<buffer_type>::append ( buffer ); \
  return *this; \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro used in derived classes to implement operator +=.
//
///////////////////////////////////////////////////////////////////////////////

#define SL_TEMPLATE_STRING_PLUS_EQUALS_OPERATOR(class_name,data_type) \
inline class_name &class_name::operator += ( data_type data ) \
{ \
  return this->append ( data ); \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro used in derived classes to implement operator <<.
//
///////////////////////////////////////////////////////////////////////////////

#define SL_TEMPLATE_STRING_OUTPUT_OPERATOR(class_name,data_type) \
inline class_name &operator << ( class_name &lhs, data_type data ) \
{ \
  return lhs.append ( data ); \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Recommended buffer size. Use with above macros.
//
///////////////////////////////////////////////////////////////////////////////

#define SL_STRING_FUNCTION_BUFFER_SIZE 32768 // 2^15


#ifdef CADKIT_DEFINE_SL_TEMPLATE_STRING_SET_FROM_ERROR_ID


///////////////////////////////////////////////////////////////////////////////
//
// Set the string from the given error id. This function is not a member.
//
///////////////////////////////////////////////////////////////////////////////

template<class F, class S, class B> inline bool _setFromErrorId ( const HRESULT &error, F function, S theString, B buffer )
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
      theString.setValue ( buffer );

      // Free the string.
      ::LocalFree ( buffer );

      // It worked.
      return true;
    }
  }

  // If we get to here then it didn't work (probably a non-standard error id).
  return false;
}


#endif // CADKIT_DEFINE_SL_TEMPLATE_STRING_SET_FROM_ERROR_ID


}; // namespace CadKit.


#endif
