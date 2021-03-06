
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Trim policy classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_POLICY_TRIM_H_
#define _XML_POLICY_TRIM_H_

#include <locale>


namespace XML {
namespace Config {


///////////////////////////////////////////////////////////////////////////////
//
//  Class to trim the value-string.
//
///////////////////////////////////////////////////////////////////////////////

struct Trim
{
  template < class String > void operator () ( String &s )
  {
    // Default locale.
    std::locale loc;

    // Lose leading white space.
    while ( !s.empty() && std::isspace ( s[0], loc ) )
      s.erase ( s.begin() );

    // Lose trailing white space.
    while ( !s.empty() && std::isspace ( s[s.size()-1], loc ) )
      s.erase ( s.size() - 1 );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Class that doesn't trim the value-string.
//
///////////////////////////////////////////////////////////////////////////////

struct NoTrim
{
  template < class String > void operator () ( String &s ){}
};


}; // namespace Config
}; // namespace XML


#endif // _XML_POLICY_TRIM_H_
