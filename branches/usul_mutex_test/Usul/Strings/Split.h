
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Split string.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_STRINGS_SPLIT_H_
#define _USUL_STRINGS_SPLIT_H_

#include <algorithm>
#include <string>


namespace Usul {
namespace Strings {


///////////////////////////////////////////////////////////////////////////////
//
//  Adopted from:
//  http://www.we11er.co.uk/blog/2005/03/stdstring-sucks_111177343999267629.html
//
///////////////////////////////////////////////////////////////////////////////

template < class StringContainer > void split ( const std::string &input, const std::string &delim, bool wantEmptyStrings, StringContainer &output )
{
  // Find index of delimiter.
  std::string::size_type offset ( 0 );
  std::string::size_type index ( input.find ( delim, offset ) );

  // Used in the loop.
  std::string sub;

  // While we are not at the end...
  while ( index != std::string::npos )
  {
    // Find sub-string.
    sub = input.substr ( offset, index - offset );

    // Insert into container if we should.
    if ( false == sub.empty() || wantEmptyStrings )
      output.insert ( output.end(), sub );

    // Find next delimiter.
    offset += ( index - offset + delim.length() );
    index = input.find ( delim, offset );
  }

  // Insert last sub-string if we should.
  sub = input.substr ( offset );
  if ( false == sub.empty() || wantEmptyStrings )
    output.insert ( output.end(), sub );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Split the string.
//
///////////////////////////////////////////////////////////////////////////////

template < class StringContainer > void split ( const std::string &input, char delim, bool wantEmptyStrings, StringContainer &output )
{
  const std::string d ( 1, delim );
  Usul::Strings::split ( input, d, wantEmptyStrings, output );
}


} // namespace Strings
} // namespace Usul


#endif // _USUL_STRINGS_SPLIT_H_
