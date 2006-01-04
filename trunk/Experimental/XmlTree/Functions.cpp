
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

#include "XmlTree/Functions.h"

#include <sstream>
#include <memory>

using namespace XmlTree;


///////////////////////////////////////////////////////////////////////////////
//
//  Format appropriate message.
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::format ( const xercesc::DOMException &e, const std::string &file, const std::string &action )
{
  const std::string message ( XmlTree::Functions::message ( e ) );
  std::ostringstream out;
  out << "Failed to " << action << " XML file '" << file << "', code: " << e.code;
  if ( false == message.empty() )
    out << ", " << message;
  return out.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the message from the exception.
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::message ( const xercesc::DOMException &e )
{
  const unsigned int size ( 2047 );
  XMLCh text [ size + 1 ];
  const bool result ( xercesc::DOMImplementation::loadDOMExceptionMsg ( e.code, text, size ) );
  return ( ( result ) ? std::string ( XmlTree::Functions::translate ( text ) ) : std::string() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the message from the exception.
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::message ( const xercesc::XMLException &e )
{
  return ( ( e.getMessage() ) ? XmlTree::Functions::translate ( e.getMessage() ) : std::string() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name of the node.
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::name ( const xercesc::DOMNode *node )
{
  if ( 0x0 != node )
  {
    switch ( node->getNodeType() )
    {
      case xercesc::DOMNode::ELEMENT_NODE:
      case xercesc::DOMNode::ATTRIBUTE_NODE:
        return ( XmlTree::Functions::translate ( node->getNodeName() ) );
    }
  }
  return std::string();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value of the node.
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::value ( const xercesc::DOMNode *node )
{
  if ( 0x0 != node )
  {
    switch ( node->getNodeType() )
    {
      case xercesc::DOMNode::TEXT_NODE:
      case xercesc::DOMNode::ATTRIBUTE_NODE:
        return ( XmlTree::Functions::translate ( node->getNodeValue() ) );
    }
  }
  return std::string();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate the unicode string.
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::translate ( const XMLCh *text )
{
  // Initialize.
  std::string s;

  // If there is text...
  if ( text )
  {
    // The translate function allocates with "new".
    std::auto_ptr<const char> translated ( xercesc::XMLString::transcode ( text ) );
    if ( translated.get() )
    {
      // Set the string.
      s = translated.get();
    }
  }

  // Return what we have.
  return s;
}
