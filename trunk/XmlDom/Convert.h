
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Convenience classes to convert strings to other types.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_CONVERT_STRING_H_
#define _XML_CONVERT_STRING_H_

#include <sstream>
#include <algorithm>


namespace XML {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic definition.
//
///////////////////////////////////////////////////////////////////////////////

template < class To > struct Convert
{
  template < class From > bool operator () ( const From &from )
  {
    typedef typename From::value_type CharType;
    std::basic_istringstream<CharType> in ( from );
    To value;
    in >> value;
    return value;
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Convert to a boolean.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct Convert < bool >
{
  template < class From > bool operator () ( const From &from )
  {
    typedef typename From::value_type CharType;
    std::basic_istringstream<CharType> in ( from );
    From temp;
    in >> temp; // Trim off white space.
    std::transform ( temp.begin(), temp.end(), temp.begin(), ::toupper );
    return ( temp == "TRUE" || temp == "1" ) ? true : false;
  }
};


}; // namespace XML


#endif // _XML_CONVERT_STRING_H_
