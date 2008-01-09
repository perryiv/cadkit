
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SERIALIZE_XML_STANDARD_STRING_H__
#define __SERIALIZE_XML_STANDARD_STRING_H__

#include "XmlTree/Node.h"
#include "XmlTree/ReplaceIllegalCharacters.h"

#include <string>

namespace Serialize {
namespace XML {


inline void serialize ( const std::string& name, XmlTree::Node &parent, const std::string& value )
{
  // Pre-process string to remove illegal XML characters.
  // Does nothing for non-string types.
  std::string temp ( value );
  XmlTree::replaceIllegalCharacters ( temp );

  // Make node with processed string.
  parent.children().push_back ( new XmlTree::Node ( name, temp ) );
}


inline void deserialize ( const std::string& name, const XmlTree::Node &node, std::string& value )
{
  if ( name == node.name() )
  {
    // Post-process string to swap illegal XML characters.
    // Does nothing for non-string types.
    std::string out ( node.value() );
    XmlTree::restoreIllegalCharacters ( out );

    // Set value from string.
    value = out;
  }
}

}
}

#endif // __SERIALIZE_XML_STANDARD_STRING_H__
