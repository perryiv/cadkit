
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlStringFunctions: Functions to compare strings.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _CADKIT_STANDARD_LIBRARY_STRING_COMPARE_FUNCTIONS_H_
#define _CADKIT_STANDARD_LIBRARY_STRING_COMPARE_FUNCTIONS_H_

#include "SlStringCase.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  See if the given strings are equal.
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


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_STRING_COMPARE_FUNCTIONS_H_
