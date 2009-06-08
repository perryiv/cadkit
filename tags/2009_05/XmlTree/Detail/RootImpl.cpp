
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  XML tree root class.
//
///////////////////////////////////////////////////////////////////////////////

#include "XmlTree/Detail/RootImpl.h"

#include "Usul/Errors/Assert.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Strings/Split.h"

#include "xercesc/dom/DOMImplementationRegistry.hpp"
#include "xercesc/dom/DOMWriter.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include "xercesc/framework/LocalFileFormatTarget.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/util/OutOfMemoryException.hpp"

#include <stdexcept>
#include <memory>
#include <list>

using namespace XmlTree::Detail;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RootImpl::RootImpl() : _document()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

RootImpl::~RootImpl()
{
  this->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the tree.
//
///////////////////////////////////////////////////////////////////////////////

void RootImpl::clear()
{
  if ( _document )
  {
    _document->release();
    _document = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the file.
//
///////////////////////////////////////////////////////////////////////////////

void RootImpl::load ( const std::string &file )
{
  // Check file existance.
  {
    Usul::Predicates::FileExists exists;
    if ( false == exists ( file ) )
      throw std::runtime_error ( "Error 1297101375: Given file '" + file + "' does not exist" );
  }

  // Make new parser.
  std::auto_ptr<xercesc::XercesDOMParser> parser ( new xercesc::XercesDOMParser );
  parser->setValidationScheme ( xercesc::XercesDOMParser::Val_Always );

  // Safely...
  try
  {
    // Parse the file.
    parser->parse ( file.c_str() );

    // Clear existing tree.
    this->clear();

    // Take over ownership of document. Otherwise, the parser deletes it.
    _document = parser->adoptDocument();
  }

  // Catch and re-throw exceptions.
  catch ( const xercesc::OutOfMemoryException & )
  {
    Usul::Exceptions::Thrower<std::runtime_error> ( "Error 7706621990: Ran out of memory while parsing XML file: ", file );
  }
  catch ( const xercesc::XMLException &e )
  {
    Usul::Exceptions::Thrower<std::runtime_error> ( "Error 2262031876: Failed to parse XML file '", file, "', ", ( ( e.getMessage() ) ? xercesc::XMLString::transcode ( e.getMessage() ) : "Unknown error" ) );
  }
  catch ( const xercesc::DOMException &e )
  {
    Usul::Exceptions::Thrower<std::runtime_error> ( this->_format ( e, file ) );
  }
  catch ( ... )
  {
    Usul::Exceptions::Thrower<std::runtime_error> ( "Error 3432411620: Unknown exception caught while parsing XML file: ", file );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Format the error string.
//
///////////////////////////////////////////////////////////////////////////////

std::string RootImpl::_format ( const xercesc::DOMException &e, const std::string &file )
{
  const unsigned int maxChars ( 2047 );
  XMLCh errText[maxChars + 1];
  std::ostringstream out;
  out << "Error 4169307617: Failed to parse XML file '" << file << "', code: " << e.code;
  if ( xercesc::DOMImplementation::loadDOMExceptionMsg ( e.code, errText, maxChars ) )
    out << ", " << xercesc::XMLString::transcode ( errText );
  return out.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write to file.
//
///////////////////////////////////////////////////////////////////////////////

void RootImpl::write ( const std::string &file )
{
  // Make sure there is a document.
  this->_ensureDocument();

  // Make DOMWriter.
  const unsigned int size ( 128 );
  XMLCh temp[size];
  xercesc::XMLString::transcode ( "LS", temp, size - 1 );
  xercesc::DOMImplementation *impl = xercesc::DOMImplementationRegistry::getDOMImplementation ( temp );
  xercesc::DOMImplementationLS *ls ( dynamic_cast < xercesc::DOMImplementationLS * > ( impl ) );
  std::auto_ptr<xercesc::DOMWriter> writer ( ( ls ) ? ls->createDOMWriter() : 0x0 );

  // Did it work?
  if ( 0x0 == writer.get() )
    throw std::runtime_error ( "Error 2454091518: Failed to create DOM writer" );

  // Make file target.
  std::auto_ptr<xercesc::XMLFormatTarget> target ( new xercesc::LocalFileFormatTarget ( file.c_str() ) );

  // Use only newline characters.
  XMLCh newLine[] = { 'L', 'F', '\0' };
  writer->setNewLine ( newLine );

  // Write the document.
  writer->writeNode ( target.get(), *_document );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the node.
//
///////////////////////////////////////////////////////////////////////////////

void RootImpl::node ( const std::string &path, char delim, unsigned int value )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the node.
//
///////////////////////////////////////////////////////////////////////////////

void RootImpl::node ( const std::string &path, char delim, int value )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the node.
//
///////////////////////////////////////////////////////////////////////////////

void RootImpl::node ( const std::string &path, char delim, bool value )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the node.
//
///////////////////////////////////////////////////////////////////////////////

void RootImpl::node ( const std::string &path, char delim, const std::string &value )
{
  // Make sure there is a document.
  this->_ensureDocument();

  // Split path.
  typedef std::list < std::string > Parts;
  Parts parts;
  Usul::Strings::split ( path, delim, false, parts );

  // Get list of nodes.
  //xercesc::DOMNodeList *kids ( _document->getChildNodes() );

  // Loop through nodes.
  //const XMLSize_t num ( kids->getLength() );
//  for ( xercesc::XMLSize_t i = 0; i < num; ++i )
//    HERE
//    Node, need to pass in an unsigner integer indicating which occurance of the path to operate on.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a document if there isn't one.
//
///////////////////////////////////////////////////////////////////////////////

void RootImpl::_ensureDocument()
{
  // If there is no document then make one.
  if ( 0x0 == _document )
  {
    // Untested code between these brackets. Remove this assertion if it works.
    USUL_ASSERT ( 0 );

    // Make unicode strings.
    const unsigned int size ( 128 );
    XMLCh core[size], root[size];
    xercesc::XMLString::transcode ( "Core", core, size - 1 );
    xercesc::XMLString::transcode ( "root", root, size - 1 );

    // Make document.
    xercesc::DOMImplementation *impl ( xercesc::DOMImplementationRegistry::getDOMImplementation ( core ) );
    _document = ( ( impl ) ? impl->createDocument ( 0x0, root, 0x0 ) : 0x0 );

    // Did it work?
    if ( 0x0 == _document )
      throw std::runtime_error ( "Error 4024167305: could not create new DOM document" );
  }
}
