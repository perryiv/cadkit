
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
#include "SlStringReplace.h"

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

  typedef typename std::basic_string<CharType> StringType;
  typedef typename StringType::size_type       SizeType;
  typedef CharType                                     value_type;
  typedef const value_type *                           const_iterator;
  typedef value_type *                                 iterator;
  
  static const typename StringType::size_type npos;

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

  // Clear the string, redundancy is for compatibility with template functions.
  void                          clear() { _s.erase ( _s.begin(), _s.end() ); }

  // Erase the elements.
  iterator                      erase ( iterator first, iterator last ) { return _s.erase ( first, last ); }
  iterator                      erase ( iterator it ) { return _s.erase ( it ); } // STLport debug doesn't like.
  StringType &                  erase ( unsigned int p0 = 0, unsigned int n = npos) { return _s.erase ( p0, n ); }

  // Iterators.
  const_iterator                begin() const { return _s.begin(); }
  iterator                      begin()       { return _s.begin(); }
  const_iterator                end()   const { return _s.end(); }
  iterator                      end()         { return _s.end(); }

  // Get the character.
  const CharType &              getChar ( const unsigned int &index ) const;
  CharType &                    getChar ( const unsigned int &index );

  // For compatibility with basic_string.
  const CharType &              at ( const unsigned int &index ) const { return this->getChar ( index ); }
  CharType &                    at ( const unsigned int &index )       { return this->getChar ( index ); }

  // Get the internal string.
  const CharType *              c_str()     const { return _s.c_str(); }
  const StringType &            getString() const { return _s; }
  StringType &                  getString()       { return _s; }

  // Return the first and last letters, or '\0' if the string is empty.
  CharType                      getFirst() const { return ( _s.empty() ) ? ((CharType)0) : _s.at ( 0 ); }
  CharType                      getLast()  const { return ( _s.empty() ) ? ((CharType)0) : _s.at ( _s.size() - 1 ); }

  // Get the length, redundancy is for compatibility with template functions.
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
  SlString &                    operator = ( const SlString &rhs )   { this->setString ( rhs ); return *this; }
  SlString &                    operator = ( const StringType &rhs ) { this->setString ( rhs ); return *this; }
  SlString &                    operator = ( const CharType *rhs )   { this->setString ( rhs ); return *this; }
  SlString &                    operator = ( CharType rhs )          { this->setString ( rhs ); return *this; }

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

  // Replace all occurances of the one character (or substring) with the other.
  void                          replace ( const CharType &oldChar, const CharType &newChar );
  void                          replace ( const SlString &oldSubString, const SlString &newSubString );

  // Reverse the order of the string.
  void                          reverse();

  // Set the value.
  void                          setString ( const SlString &str )   { _s = str._s; }
  void                          setString ( const StringType &str ) { _s = str; }
  void                          setString ( const CharType *str )   { _s = str; }
  void                          setString ( CharType c )            { _s = c; }

protected:

  StringType _s;
};


// Define npos.
template<class CharType>
const typename SlString<CharType>::SizeType SlString<CharType>::npos = -1;


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
  CadKit::replaceChars ( oldChar, newChar, _s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Replace the substring.
//
///////////////////////////////////////////////////////////////////////////////

template<class CharType> inline void SlString<CharType>::replace 
  ( const SlString<CharType> &oldSubString, const SlString<CharType> &newSubString )
{
  CadKit::replaceSubStrings ( oldSubString._s, newSubString._s, _s );
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


#endif // _CADKIT_STANDARD_LIBRARY_TEMPLATE_STRING_CLASS_
