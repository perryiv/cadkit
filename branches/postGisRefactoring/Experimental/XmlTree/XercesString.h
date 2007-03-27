
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions for working with xerces strings.
//  From C++ Cookbook.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __XERCES_STRINGS_H__
#define __XERCES_STRINGS_H__

#include "boost/scoped_array.hpp"
#include "xercesc/util/XMLString.hpp"

#include <string>

namespace XmlTree 
{
  typedef std::basic_string<XMLCh> XercesString;


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Converts narrow-character string to wide-character string.
  //
  ///////////////////////////////////////////////////////////////////////////////

  inline XercesString fromNative( const char *str )
  {
    boost::scoped_array < XMLCh > ptr ( xercesc::XMLString::transcode ( str ) );
    return XercesString( ptr.get () );
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Converts narrow-character string to wide-character string.
  //
  ///////////////////////////////////////////////////////////////////////////////

  inline XercesString fromNative( const std::string& str )
  {
    return XercesString( fromNative ( str.c_str() ) );
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Converts wide-character string to narrow-character string.
  //
  ///////////////////////////////////////////////////////////////////////////////

  inline std::string toNative ( const XMLCh* str )
  {
    boost::scoped_array < char > ptr ( xercesc::XMLString::transcode ( str ) );
    return std::string ( ptr.get() );
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Converts wide-character string to narrow-character string.
  //
  ///////////////////////////////////////////////////////////////////////////////

  inline std::string toNative ( const XercesString& str )
  {
    return toNative ( str.c_str() );
  }


  inline bool compare ( const XMLCh *str1, const char *str2 )
  {
    std::string lhs ( toNative( str1 ) );
    std::string rhs ( str2 );

    return lhs == rhs;
  }

}

#endif // __XERCES_STRINGS_H__

