
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __REPLACE_ILLEGAL_CHARACTERS_H__
#define __REPLACE_ILLEGAL_CHARACTERS_H__

#include <string>

#include "boost/algorithm/string/replace.hpp"

namespace XmlTree
{
  inline void replaceIllegalCharacters ( std::string& string )
  {
    boost::algorithm::replace_all ( string, "<", "&lt;" );
    boost::algorithm::replace_all ( string, ">", "&gt;" );
    boost::algorithm::replace_all ( string, "&", "&amp;" );
    boost::algorithm::replace_all ( string, "'", "&apos;" );
    boost::algorithm::replace_all ( string, "\"", "&quot;" );
  }

  inline void restoreIllegalCharacters ( std::string& string )
  {
    boost::algorithm::replace_all ( string, "&lt;", "<" );
    boost::algorithm::replace_all ( string, "&gt;", ">" );
    boost::algorithm::replace_all ( string, "&amp;", "&" );
    boost::algorithm::replace_all ( string, "&apos;", "'" );
    boost::algorithm::replace_all ( string, "&quot;", "\"" );
  }
}


#endif // __REPLACE_ILLEGAL_CHARACTERS_H__
