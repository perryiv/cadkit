
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
//  SlString: A template string that wraps std::basic_string. It is intended 
//  to add to the functionality of basic_string, and not replace it.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _CADKIT_STANDARD_LIBRARY_TEMPLATE_STRING_CLASS_
#define _CADKIT_STANDARD_LIBRARY_TEMPLATE_STRING_CLASS_

#include "SlAssert.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>    // For std::basic_string
# include <algorithm> // For std::reverse
# include <wchar.h>   // For wchar_t
#endif


namespace CadKit
{
template<class CharType> class SlString
{
public:

  typedef std::basic_string<CharType> StringType;
  typedef CharType                    value_type;
  typedef const value_type *          const_iterator;
  typedef value_type *                iterator;

  // Constructors.
  SlString(){}
  SlString ( const SlString &str )   : _s ( str._s ){}
  SlString ( const StringType &str ) : _s ( str ){}
  SlString ( const CharType *str )   : _s ( str ){}
  SlString ( CharType c )            : _s ( 1, c ){}

  // Append.
  StringType &                  append ( const SlString &str )   { _s.append ( str._s ); return *this; }
  StringType &                  append ( const StringType &str ) { _s.append ( str );    return *this; }
  StringType &                  append ( const CharType *str )   { _s.append ( str );    return *this; }
  StringType &                  append ( CharType c )            { _s.append ( 1, c );   return *this; }

  // Access.


  // Clear the string, redundancy is for compability with template functions.
  void                          clear() { _s.erase ( _s.begin(), _s.end() ); }

  // Erase the elements.
  iterator                      erase ( iterator first, iterator last ) { return _s.erase ( first, last ); }
  iterator                      erase ( iterator it ) { return _s.erase ( it ); }
  StringType &                  erase ( unsigned int p0 = 0, unsigned int n = StringType::npos ) { return _s.erase ( p0, n ); }

  // Iterators.
  const_iterator                begin() const { return _s.begin(); }
  iterator                      begin()       { return _s.begin(); }
  const_iterator                end()   const { return _s.end(); }
  iterator                      end()         { return _s.end(); }

  // Get the character.
  const CharType &              getChar ( const unsigned int &index ) const;
  CharType &                    getChar ( const unsigned int &index );

  // For compability with basic_string.
  const CharType &              at ( const unsigned int &index ) const { return this->getChar ( index ); }
  CharType &                    at ( const unsigned int &index )       { return this->getChar ( index ); }

  // Get the internal string.
  const CharType *              c_str()     const { return _s.c_str(); }
  const StringType &            getString() const { return _s; }
  StringType &                  getString()       { return _s; }

  // Return the first and last letters, or '\0' if the string is empty.
  CharType                      getFirst() const { return ( _s.empty() ) ? ((CharType)0) : _s.at ( 0 ); }
  CharType                      getLast()  const { return ( _s.empty() ) ? ((CharType)0) : _s.at ( _s.size() - 1 ); }

  // Get the length, redundancy is for compability with template functions.
  unsigned int                  getLength() const { return _s.size(); }
  unsigned int                  length() const { return _s.size(); }
  unsigned int                  size() const { return _s.size(); }

  // Is it empty?
  bool                          empty()      const { return ( _s.size() == 0 ); }
  bool                          isEmpty()    const { return ( _s.size() == 0 ); }
  bool                          isNotEmpty() const { return ( _s.size()  > 0 ); }

  // See if the strings are equal.
  bool                          isEqual ( const SlString &str ) const { return ( _s == str ); }

  // Typecast operators.
                                operator const CharType * ()   const { return _s.c_str(); }
                                operator const StringType & () const { return _s; }
                                operator       StringType & ()       { return _s; }

  // Bracket operator.
  const CharType &              operator [] ( unsigned int index ) const { return this->getChar ( index ); }
  CharType &                    operator [] ( unsigned int index )       { return this->getChar ( index ); }

  // Assignment operators.
  SlString &                    operator = ( const SlString &rhs )   { this->setValue ( rhs ); return *this; }
  SlString &                    operator = ( const StringType &rhs ) { this->setValue ( rhs ); return *this; }
  SlString &                    operator = ( const CharType *rhs )   { this->setValue ( rhs ); return *this; }
  SlString &                    operator = ( CharType rhs )          { this->setValue ( rhs ); return *this; }

  // Append operators.
  SlString &                    operator += ( const SlString &rhs )   { this->append ( rhs ); return *this; }
  SlString &                    operator += ( const StringType &rhs ) { this->append ( rhs ); return *this; }
  SlString &                    operator += ( const CharType *rhs )   { this->append ( rhs ); return *this; }
  SlString &                    operator += ( CharType rhs )          { this->append ( rhs ); return *this; }

  // Pop a character off the string.
  void                          pop_back()  { if ( this->isNotEmpty() ) { _s.erase ( _s.size() - 1, 1 ); } }
  void                          pop_front() { if ( this->isNotEmpty() ) { _s.erase ( _s.begin() ); } }

  // Push a character onto the string.
  void                          push_back  ( const CharType &c ) { _s.insert ( _s.end(),   c ); }
  void                          push_front ( const CharType &c ) { _s.insert ( _s.begin(), c ); }

  // Replace all occurances of the one character with the other.
  void                          replace ( const CharType &oldChar, const CharType &newChar );

  // Reverse the order of the string.
  void                          reverse();

  // Set the value.
  void                          setValue ( const SlString &str )   { _s = str._s; }
  void                          setValue ( const StringType &str ) { _s = str; }
  void                          setValue ( const CharType *str )   { _s = str; }
  void                          setValue ( CharType c )            { _s = c; }

protected:

  StringType _s;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Get the character.
//
///////////////////////////////////////////////////////////////////////////////

template<class CharType> inline const CharType &SlString<CharType>::getChar 
  ( const unsigned int &index ) const
{
  SL_ASSERT ( this && index < this->getLength() );
  return _s.at ( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the character.
//
///////////////////////////////////////////////////////////////////////////////

template<class CharType> inline CharType &SlString<CharType>::getChar
  ( const unsigned int &index )
{
  SL_ASSERT ( this && index < this->getLength() );
  return _s.at ( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Replace all occurances of the one character with the other.
//
///////////////////////////////////////////////////////////////////////////////

template<class CharType> inline void SlString<CharType>::replace 
  ( const CharType &oldChar, const CharType &newChar )
{
  // Loop through all the characters.
  unsigned int length = this->getLength();
  for ( unsigned int i = 0; i < length; ++i )
  {
    // If the current character is the right one then replace it.
    if ( oldChar == (*this)[i] ) 
      (*this)[i] = newChar;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reverse the order of the string.
//
///////////////////////////////////////////////////////////////////////////////

template<class CharType> inline void SlString<CharType>::reverse()
{
  std::reverse ( _s.begin(), _s.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Common types.
//
///////////////////////////////////////////////////////////////////////////////

typedef SlString<char>    SlAString;
typedef SlString<wchar_t> SlWString;


}; // namespace CadKit.


#endif // CADKIT_DEFINE_SL_TEMPLATE_STRING_SET_FROM_ERROR_ID
