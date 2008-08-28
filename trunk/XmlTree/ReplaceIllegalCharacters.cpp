
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "ReplaceIllegalCharacters.h"

#include "boost/algorithm/string/replace.hpp"


///////////////////////////////////////////////////////////////////////////////
//
//  Replace illegal xml characters
//
///////////////////////////////////////////////////////////////////////////////

std::string XmlTree::translateToLegalCharacters ( const std::string& s )
{
  std::string temp ( s );
  XmlTree::replaceIllegalCharacters ( temp );
  return temp;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Replace illegal xml characters
//
///////////////////////////////////////////////////////////////////////////////

void XmlTree::replaceIllegalCharacters ( std::string& string )
{
  // Replace & first to ensure proper conversion.
  boost::algorithm::replace_all ( string, "&", "&amp;" );
  boost::algorithm::replace_all ( string, "<", "&lt;" );
  boost::algorithm::replace_all ( string, ">", "&gt;" );
  boost::algorithm::replace_all ( string, "'", "&apos;" );
  boost::algorithm::replace_all ( string, "©", "&copy;" );
  boost::algorithm::replace_all ( string, "\"", "&quot;" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Restore illegal xml characters
//
///////////////////////////////////////////////////////////////////////////////

void XmlTree::restoreIllegalCharacters ( std::string& string )
{
  // Replace & last to ensure proper conversion.
  boost::algorithm::replace_all ( string, "&lt;", "<" );
  boost::algorithm::replace_all ( string, "&gt;", ">" );
  boost::algorithm::replace_all ( string, "&apos;", "'" );
  boost::algorithm::replace_all ( string, "&copy;", "©" );
  boost::algorithm::replace_all ( string, "&quot;", "\"" );
  boost::algorithm::replace_all ( string, "&amp;", "&" );
}
