
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlStringTrim: Functions for trimming strings.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _CADKIT_STANDARD_LIBRARY_TEMPLATE_STRING_TRIM_CLASS_
#define _CADKIT_STANDARD_LIBRARY_TEMPLATE_STRING_TRIM_CLASS_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Trim the beginning of the string. Works best when StringType is a 
//  std::basic_string.
//
///////////////////////////////////////////////////////////////////////////////

template<class CharType, class StringType> inline bool trimStringBeginning (
  const CharType &trimMe, 
  StringType &string )
{
  // Handle trivial case.
  if ( 0 == string.size() )
    return false;

  // Loop until there is nothing left.
  while ( string.size() )
  {
    // See if the beginning of the string is the given character.
    // If not then we are done.
    if ( trimMe != string[0] )
      return true;

    // Erase the first character.
    string.erase ( 0, 1 );
  }

  // Should never get here.
  SL_ASSERT ( 0 );

  // Assume the string was trimmed.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Trim the end of the string. Works best when StringType is a 
//  std::basic_string.
//
///////////////////////////////////////////////////////////////////////////////

template<class CharType, class StringType> inline bool trimStringEnd (
  const CharType &trimMe, 
  StringType &string )
{
  // Handle trivial case.
  if ( 0 == string.size() )
    return false;

  // Needed below.
  unsigned int size ( 0 );

  // Loop until there is nothing left.
  while ( string.size() )
  {
    // Get the size.
    size = string.size();

    // See if the beginning of the string is the given character.
    // If not then we are done.
    if ( trimMe != string[size - 1] )
      return true;

    // Erase the last character.
    string.erase ( size - 1, 1 );
  }

  // Should never get here.
  SL_ASSERT ( 0 );

  // Assume the string was trimmed.
  return true;
}


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_TEMPLATE_STRING_TRIM_CLASS_
