
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  XML loader class.
//
///////////////////////////////////////////////////////////////////////////////

#include "XmlTree/Loader.h"
#include "XmlTree/Functions.h"
#include "XmlTree/Document.h"
#include "XmlTree/XercesLife.h"

#include "Usul/Errors/Assert.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/Predicates/FileExists.h"

#include "xercesc/dom/DOM.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/util/OutOfMemoryException.hpp"

#include <stdexcept>
#include <memory>
#include <iostream>

using namespace XmlTree;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Loader::Loader()
{
  XmlTree::XercesLife::initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Loader::~Loader()
{
  try
  {
    XercesLife::terminate();
  }
  catch ( const std::exception &e )
  {
    std::cout << e.what() << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to load contents of file and return the document.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  xercesc::DOMDocument *load ( const std::string &file )
  {
    // Check file existance.
    {
      Usul::Predicates::FileExists exists;
      if ( false == exists ( file ) )
        throw std::runtime_error ( "Error 1297101375: Given file '" + file + "' does not exist" );
    }

    // Safely...
    try
    {
      // Make new parser.
      std::auto_ptr<xercesc::XercesDOMParser> parser ( new xercesc::XercesDOMParser );
      parser->setValidationScheme ( xercesc::XercesDOMParser::Val_Always );

      // Parse the file.
      parser->parse ( file.c_str() );

      // Take over ownership of document and return it.
      return parser->adoptDocument();
    }

    // Catch and re-throw exceptions.
    catch ( const xercesc::OutOfMemoryException & )
    {
      Usul::Exceptions::Thrower<std::runtime_error> 
        ( "Error 1471545006: Ran out of memory while parsing XML file: ", file );
    }
    catch ( const xercesc::XMLException &e )
    {
      Usul::Exceptions::Thrower<std::runtime_error> 
        ( "Error 6547875590: Failed to parse XML file '", file, "', ", XmlTree::Functions::translate ( e.getMessage() ) );
    }
    catch ( const xercesc::DOMException &e )
    {
      Usul::Exceptions::Thrower<std::runtime_error> 
        ( "Error 2777877755: ", XmlTree::Functions::format ( e, file, "load" ) );
    }
    catch ( ... )
    {
      Usul::Exceptions::Thrower<std::runtime_error> 
        ( "Error 1638328027: Unknown exception caught while parsing XML file: ", file );
    }

    // The compiler does not realize that we will never get here.
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set attributes.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setAttributes ( const xercesc::DOMNamedNodeMap *attribs, XmlTree::Node *node )
  {
    if ( attribs && node )
    {
      // Loop through the attributes.
      const XMLSize_t num ( attribs->getLength() );
      for ( XMLSize_t i = 0; i < num; ++i )
      {
        // Attributes are abstracted as "nodes"... weird.
        xercesc::DOMNode *attr ( attribs->item ( i ) );
        if ( attr )
        {
          const std::string name  ( XmlTree::Functions::name  ( attr ) );
          const std::string value ( XmlTree::Functions::value ( attr ) );
          node->attributes()[name] = value;
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prototype needed below.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper { void traverse ( const xercesc::DOMNode *, XmlTree::Node * ); }


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to traverse the children.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void traverse ( const xercesc::DOMNodeList *kids, XmlTree::Node *parent )
  {
    // Get number of children.
    const XMLSize_t num ( ( kids ) ? kids->getLength() : 0 );

    // Loop through the children.
    for ( XMLSize_t i = 0; i < num; ++i )
    {
      // Get the child node.
      xercesc::DOMNode *kid ( kids->item ( i ) );
      if ( kid )
      {
        // Make a new child.
        XmlTree::Node::ValidRefPtr child ( new XmlTree::Node ( XmlTree::Functions::name ( kid ) ) );

        // Call this function for the new child node.
        Helper::traverse ( kid, child.get() );

        // Append the child.
        parent->children().push_back ( child );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to identify our definition of a text node.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  bool isTextNode ( const xercesc::DOMNode *dom )
  {
    if ( dom )
    {
      const xercesc::DOMNodeList *kids ( dom->getChildNodes() );
      const XMLSize_t num ( ( kids ) ? kids->getLength() : 0 );
      if ( 1 == num )
      {
        const xercesc::DOMNode *kid ( kids->item ( 0 ) );
        return ( xercesc::DOMNode::TEXT_NODE == kid->getNodeType() );
      }
    }
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to traverse the DOM node and add children to our node.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void traverse ( const xercesc::DOMNode *dom, XmlTree::Node *node )
  {
    // Check input.
    if ( 0x0 == dom || 0x0 == node )
      Usul::Exceptions::Thrower<std::runtime_error> ( "Error 3066136826: null node found while traversing" );

    // Get the type.
    const short type ( dom->getNodeType() );

    // Is it a text node?
    if ( Helper::isTextNode ( dom ) )
    {
      // Set the value and return.
      node->value ( XmlTree::Functions::value ( dom ) );
      return;
    }

    // Otherwise, if it is not an element...
    if ( xercesc::DOMNode::ELEMENT_NODE != type )
    {
      // Warn the user and return.
      std::cout << "Warning 1746255179: skipping unknown node type '" << type << "' found while traversing DOM tree" << std::endl;
      return;
    }

    // Set the attributes.
    Helper::setAttributes ( dom->getAttributes(), node );

    // Traverse children.
    Helper::traverse ( dom->getChildNodes(), node );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load contents of file and build the document. Node: in Xerces, a "node" 
//  is everything between the tags, an "element" is a node with children, 
//  and "text" is a simple leaf node.
//
///////////////////////////////////////////////////////////////////////////////

void Loader::load ( const std::string &file, Document *doc )
{
  // Load document.
  std::auto_ptr<xercesc::DOMDocument> dom ( Helper::load ( file ) );
  if ( 0x0 == dom.get() )
      Usul::Exceptions::Thrower<std::runtime_error> ( "Error 1221774659: Failed to create DOM document from XML file: ", file );

  // Get document element.
  xercesc::DOMElement *root ( dom.get()->getDocumentElement() );

  // Name the document.
  doc->name ( XmlTree::Functions::name ( root ) );

  // Traverse the tree and make our nodes.
  Helper::traverse ( root, doc );
}
