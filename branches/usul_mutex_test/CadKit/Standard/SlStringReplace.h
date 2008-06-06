
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlStringReplace: Functions for replacing substrings.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _CADKIT_STANDARD_LIBRARY_TEMPLATE_STRING_REPLACE_CLASS_
#define _CADKIT_STANDARD_LIBRARY_TEMPLATE_STRING_REPLACE_CLASS_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Replace all occurances of the one character with the other.
//
///////////////////////////////////////////////////////////////////////////////

template<class CharType, class StringType> inline void replaceChars (
  const CharType &oldChar, 
  const CharType &newChar, 
  StringType &string )
{
  // Loop through all the characters.
  unsigned int length = string.size();
  for ( unsigned int i = 0; i < length; ++i )
  {
    // If the current character is the right one then replace it.
    if ( oldChar == string[i] ) 
      string[i] = newChar;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Replace all occurances of the one substring with the other. Works best 
//  when StringType is a std::basic_string.
//
///////////////////////////////////////////////////////////////////////////////

template<class StringType> inline void replaceSubStrings (
  const StringType &oldSubString, 
  const StringType &newSubString,
  StringType &string )
{
  // Initialize.
  typename StringType::size_type position ( 0 );

  // Loop forever.
  while ( true )
  {
    // Get the position of the substring. Make sure we start at the end 
    // of the previously inserted substring. Otherwise, if the new substring 
    // contains some of the old substring, you create an infinite loop.
    position = string.find ( oldSubString, position );

    // If the position is the end then we're done.
    if ( StringType::npos == position )
      break;

    // Erase the old substring and insert the new one. Note: by doing this 
    // instead of overwriting we permit replacement with substrings of 
    // unequal length.
    string.erase ( position, oldSubString.size() );
    string.insert ( position, newSubString );

    // Update the position to the end of the substring we just inserted.
    position += newSubString.size();
  }
}


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_TEMPLATE_STRING_REPLACE_CLASS_
