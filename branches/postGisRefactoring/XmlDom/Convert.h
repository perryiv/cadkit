
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
  template < class From > To operator () ( const From &from )
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


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the vector.
//
///////////////////////////////////////////////////////////////////////////////

template < class To > struct ConvertVec2
{
  template < class From > To operator () ( const From &from )
  {
    typedef typename From::value_type CharType;
    std::basic_istringstream<CharType> in ( from );
    To value;
    in >> value[0] >> value[1];
    return value;
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the vector.
//
///////////////////////////////////////////////////////////////////////////////

template < class To > struct ConvertVec3
{
  template < class From > To operator () ( const From &from )
  {
    typedef typename From::value_type CharType;
    std::basic_istringstream<CharType> in ( from );
    To value;
    in >> value[0] >> value[1] >> value[2];
    return value;
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the vector.
//
///////////////////////////////////////////////////////////////////////////////

template < class To > struct ConvertVec4
{
  template < class From > To operator () ( const From &from )
  {
    typedef typename From::value_type CharType;
    std::basic_istringstream<CharType> in ( from );
    To value;
    in >> value[0] >> value[1] >> value[2] >> value[3];
    return value;
  }
};


}; // namespace XML


#endif // _XML_CONVERT_STRING_H_
