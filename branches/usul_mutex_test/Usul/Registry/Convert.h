
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Perry Miller
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  String conversion functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_REGISTRY_STRING_CONVERSIONS_H_
#define _USUL_REGISTRY_STRING_CONVERSIONS_H_

#include "Usul/Convert/Generic.h"
#include "Usul/Strings/Split.h"

#include <list>
#include <sstream>
#include <string>


namespace Usul {
namespace Convert {


///////////////////////////////////////////////////////////////////////////////
//
//  Converter for a list of strings.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct Type < std::list < std::string >, std::string >
{
  typedef std::list < std::string > StringList;

  static std::string convert ( const StringList &sl )
  {
    if ( true == sl.empty() )
      return "";

    std::ostringstream out;
    StringList::const_iterator i ( sl.begin() );
    out << ( *i ); ++i;
    for ( ; i != sl.end(); ++i )
      out << ';' << ( *i );

    return out.str();
  }
};
template <> struct Type < std::string, std::list < std::string > >
{
  typedef std::list < std::string > StringList;

  static void convert ( const std::string &s, StringList &sl )
  {
    sl.clear();

    if ( true == s.empty() )
      return;

    Usul::Strings::split ( s, ';', true, sl );
  }
};


} // namespace Convert
} // namespace Usul


#endif // _USUL_REGISTRY_STRING_CONVERSIONS_H_
