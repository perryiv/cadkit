
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
#include "XmlTree/XercesString.h"

#include "Usul/Exceptions/Thrower.h"
#include "Usul/File/Stats.h"
#include "Usul/Predicates/FileExists.h"

#include "xercesc/dom/DOM.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/util/OutOfMemoryException.hpp"
#include "xercesc/framework/LocalFileInputSource.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"

#include <stdexcept>
#include <memory>
#include <iostream>
#include <iterator>

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
  // Calling this is not thread-safe.
  /*try
  {
    XercesLife::terminate();
  }
  catch ( const std::exception &e )
  {
    std::cout << e.what() << std::endl;
  }*/
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to load contents of file and return the document.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  xercesc::DOMDocument *load ( xercesc::InputSource& input )
  {
    // Make new parser.
    std::auto_ptr<xercesc::XercesDOMParser> parser ( new xercesc::XercesDOMParser );
    parser->setValidationScheme ( xercesc::XercesDOMParser::Val_Always );

    // Parse the file.
    parser->parse ( input );

    // Normalize the document. I think this eliminates empty nodes that 
    // Xerces may create.
		xercesc::DOMDocument* doc ( parser->getDocument() );
		if ( 0x0 != doc )
			doc->normalizeDocument();

    // Take over ownership of document and return it.
    return parser->adoptDocument();
  }

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
      xercesc::LocalFileInputSource input ( XmlTree::fromNative( file ).c_str() );
      return load ( input );
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

    // Element nodes are the groups. This should always be true because of 
    // the way we decide to recursively call this function, and the way we 
    // initially call it with the document's root element.
    if ( xercesc::DOMNode::ELEMENT_NODE != dom->getNodeType() )
      Usul::Exceptions::Thrower<std::runtime_error> ( "Error 4028759980: expected element node while traversing" );

    // Should also be true.
    if ( true == node->name().empty() )
      Usul::Exceptions::Thrower<std::runtime_error> ( "Error 3856531268: found node with empty name while traversing" );

    // Set the attributes.
    Helper::setAttributes ( dom->getAttributes(), node );

    // Loop through the children.
    const xercesc::DOMNodeList *kids ( dom->getChildNodes() );
    const XMLSize_t num ( ( kids ) ? kids->getLength() : 0 );
    for ( XMLSize_t i = 0; i < num; ++i )
    {
      // Get the child.
      xercesc::DOMNode *kid ( kids->item ( i ) );

      // Is the child a text node?
      if ( kid && xercesc::DOMNode::TEXT_NODE == kid->getNodeType() )
      {
        // Xerces builds a "text node" for the value of the text, but the 
        // tag-name is always "#text". The parent "element node" will contain 
        // the tag's name. Ignore text elements that are empty.
        const std::string v ( XmlTree::Functions::value ( kid ) );
        if ( false == v.empty() )
          node->value ( v );
      }

      // Is the child an element?
      else if ( kid && xercesc::DOMNode::ELEMENT_NODE == kid->getNodeType() )
      {
        // Does it have a name?
        const std::string n ( XmlTree::Functions::name ( kid ) );
        if ( false == n.empty() )
        {
          // Call this function recursively with a new node.
          XmlTree::Node::ValidRefPtr child ( new XmlTree::Node ( n ) );
          Helper::traverse ( kid, child.get() );
          node->children().push_back ( child );
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load xercesc document into a XmlTree document.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void loadFromXercesDocument ( xercesc::DOMDocument *dom, Document * document )
  {
    // Checks for null happened elsewhere.  Assert to let us know if this changes.
    USUL_ASSERT ( 0x0 != dom );
    USUL_ASSERT ( 0x0 != document );

    // Handle no document element.
    xercesc::DOMElement *element ( dom->getDocumentElement() );
    if ( 0x0 == element )
      return;

    // Handle document's element not having a name. Only a header in the file?
    const std::string name ( XmlTree::Functions::name ( element ) );
    if ( name.empty() )
      return;

    // Set the name.
    document->name ( name );

    // Traverse the tree and make our nodes.
    Helper::traverse ( element, document );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load contents of file and build the document. 
//  
//  Note: in Xerces there is a "node" for all sections of the document, 
//  including the new-line and white-space between a closing tag at the end 
//  of one line, and an opening tag and the beginning of the next line.
//
//  We skip these empty nodes and grab the elements (groups) and text-nodes 
//  that have strings.
//
///////////////////////////////////////////////////////////////////////////////

void Loader::load ( const std::string &file, Document *doc )
{
  // A file that does not exist is an error.
  if ( false == Usul::Predicates::FileExists::test ( file ) )
    Usul::Exceptions::Thrower<std::runtime_error> ( "Error 3915685727: Given file does not exist: ", file );

  // Empty files are ok.
  if ( 0 == Usul::File::size ( file ) )
    return;

  // Load document.
  std::auto_ptr<xercesc::DOMDocument> dom ( Helper::load ( file ) );
  if ( 0x0 == dom.get() )
    Usul::Exceptions::Thrower<std::runtime_error> ( "Error 1221774659: Failed to create DOM document from XML file: ", file );

  // Populate the XmlTree::Document from the loaded xercesc document.
  Helper::loadFromXercesDocument ( dom.get(), doc );
}


///////////////////////////////////////////////////////////////////////////////
//
//  From stream.
//
///////////////////////////////////////////////////////////////////////////////

void Loader::load ( const std::istream& stream, Document *doc ) const
{
  typedef std::string Container;
  typedef std::istreambuf_iterator < Container::value_type > Iterator;

  std::istream& in ( const_cast < std::istream& > ( stream ) );
  Iterator iter ( in );

  Container contents;
  std::copy ( iter, Iterator (), std::back_inserter ( contents ) );

  this->loadFromMemory ( contents, doc );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load from a character buffer.
//
///////////////////////////////////////////////////////////////////////////////

void Loader::loadFromMemory ( const std::string& buffer, Document *doc ) const
{
  xercesc::MemBufInputSource input ( (const XMLByte*) buffer.c_str(), buffer.length(), "xml in memory parse", false  );

  // Load document.
  std::auto_ptr<xercesc::DOMDocument> dom ( Helper::load ( input ) );
  if ( 0x0 == dom.get() )
    Usul::Exceptions::Thrower<std::runtime_error> ( "Error 1390534932: Failed to create DOM document from XML." );

  // Populate the XmlTree::Document from the loaded xercesc document.
  Helper::loadFromXercesDocument ( dom.get(), doc );
}
