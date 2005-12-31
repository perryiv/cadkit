
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

#include "Usul/Exceptions/Thrower.h"
#include "Usul/Predicates/FileExists.h"

#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/util/OutOfMemoryException.hpp"

#include <stdexcept>

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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the tree.
//
///////////////////////////////////////////////////////////////////////////////

void RootImpl::clear()
{
  _document = NodePtr();
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

    // Get document.
    _document = NodePtr ( parser->getDocument() );
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
  catch (...)
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
