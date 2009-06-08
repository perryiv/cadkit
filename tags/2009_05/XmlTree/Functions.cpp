
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
#include "XmlTree/XercesString.h"

#include <sstream>
#include <memory>
#include <algorithm>
#include <ctype.h>

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
  std::string v;
  if ( 0x0 != node )
  {
    switch ( node->getNodeType() )
    {
      case xercesc::DOMNode::TEXT_NODE:
      case xercesc::DOMNode::ATTRIBUTE_NODE:
        v = XmlTree::Functions::translate ( node->getNodeValue() );
    }
  }
  return ( ( XmlTree::Functions::hasContent ( v ) ) ? v : std::string() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate the unicode string.
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::translate ( const XMLCh *text )
{
  return XmlTree::toNative ( text );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return string for node type.
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::type ( const xercesc::DOMNode *node )
{
  if ( node )
  {
    switch ( node->getNodeType() )
    {
      case xercesc::DOMNode::ELEMENT_NODE:
        return "element";
      case xercesc::DOMNode::ATTRIBUTE_NODE:
        return "attribute";
      case xercesc::DOMNode::TEXT_NODE:
        return "text";
      case xercesc::DOMNode::CDATA_SECTION_NODE:
        return "cdata_section";
      case xercesc::DOMNode::ENTITY_REFERENCE_NODE:
        return "entity_reference";
      case xercesc::DOMNode::ENTITY_NODE:
        return "entity";
      case xercesc::DOMNode::PROCESSING_INSTRUCTION_NODE:
        return "processing_instruction";
      case xercesc::DOMNode::COMMENT_NODE:
        return "comment";
      case xercesc::DOMNode::DOCUMENT_NODE:
        return "document";
      case xercesc::DOMNode::DOCUMENT_TYPE_NODE:
        return "document_type";
      case xercesc::DOMNode::DOCUMENT_FRAGMENT_NODE:
        return "document_fragment";
      case xercesc::DOMNode::NOTATION_NODE:
        return "notation";
    }
  }
  return std::string();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns true if there are characters other than white spaces.
//
///////////////////////////////////////////////////////////////////////////////

bool Functions::hasContent ( const std::string &s )
{
  for ( std::string::const_iterator i = s.begin(); i != s.end(); ++i )
  {
    const char c ( *i );
    if ( 0 == ::isspace ( c ) )
    {
      return true;
    }
  }
  return false;
}
