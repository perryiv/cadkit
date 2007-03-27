
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Misc functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_TREE_FUNCTIONS_H_
#define _XML_TREE_FUNCTIONS_H_

#include "xercesc/dom/DOM.hpp"

#include <string>


namespace XmlTree {


struct Functions
{
  // Format appropriate message.
  static std::string      format ( const xercesc::DOMException &, const std::string &file, const std::string &action );

  // Returns true if there are characters other than white spaces.
  static bool             hasContent ( const std::string & );

  // Get the message from the exception.
  static std::string      message ( const xercesc::DOMException & );
  static std::string      message ( const xercesc::XMLException & );

  // Get the name of the node.
  static std::string      name ( const xercesc::DOMNode * );

  // Translate the unicode string.
  static std::string      translate ( const XMLCh * );

  // Return string for node type.
  static std::string      type ( const xercesc::DOMNode * );

  // Get the value of the node.
  static std::string      value ( const xercesc::DOMNode * );
};


} // namespace XmlTree


#endif // _XML_TREE_FUNCTIONS_H_
