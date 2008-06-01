
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  ArcIMS layer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include "Minerva/Core/Layers/RasterLayerArcIMS.h"

#include "XmlTree/XercesLife.h"
#include "XmlTree/Document.h"
#include "XmlTree/Writer.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Temp.h"
#include "Usul/Network/Curl.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include <sstream>

using namespace Minerva::Core::Layers;

USUL_FACTORY_REGISTER_CREATOR ( RasterLayerArcIMS );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerArcIMS::RasterLayerArcIMS() : 
  BaseClass()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerArcIMS::RasterLayerArcIMS ( const RasterLayerArcIMS& rhs ) :
  BaseClass ( rhs )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerArcIMS::~RasterLayerArcIMS()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* RasterLayerArcIMS::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr clone ( new RasterLayerArcIMS ( *this ) );
  return clone.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerArcIMS::_download ( const std::string& file, const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  // Make the xml to request the image.
  std::string request ( this->_createRequestXml ( extents, width, height, level ) );

  // Make the url.
  std::string url ( this->url() + "?" + this->_getAllOptions() );

  // File to download to.
	std::string name ( Usul::File::Temp::file() );
	Usul::Scope::RemoveFile remove ( name );

  // Post the xml.
  {
    Usul::Network::Curl curl ( url, name );

    std::ostream *stream ( 0x0 );
    curl.download ( stream, request );
  }

  XmlTree::Document::RefPtr doc ( 0x0 );

  // Parse the result.  Need a guard here because XercesLife is not thread safe.  Should it contain an internal reference count?
  {
    Guard guard ( this->mutex() );
    XmlTree::XercesLife life;
    doc = new XmlTree::Document;
    doc->load ( name );
  }

  // Look for the url.
  XmlTree::Node::Children output ( doc->find ( "OUTPUT", true ) );

  // If we have an output.
  if ( false == output.empty() )
  {
    // Get the url where the image is.
    std::string imageUrl ( output.front()->attribute ( "url" ) );

    // Make sure it's not empty.
    if ( false == imageUrl.empty() )
    {
      // Download to the given filename.
      Usul::Network::Curl curl ( imageUrl, file );

      std::ostream *stream ( 0x0 );
      curl.download ( stream );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the xml to request an image.  Note that arcXml is case sensitive.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayerArcIMS::_createRequestXml ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level ) const
{
  // Typedefs.
  typedef XmlTree::Node Node;
  typedef Node::Attributes Attributes;

  // ARCXML node.
  XmlTree::Document::RefPtr arcXml ( new XmlTree::Document ( "ARCXML" ) );
  arcXml->attributes().insert ( Attributes::value_type ( "version", "1.1" ) );

  // REQUEST node.
  XmlTree::Node::RefPtr request ( arcXml->child ( "REQUEST" ) );

  // GET_IMAGE node.
  XmlTree::Node::RefPtr getImage ( request->child ( "GET_IMAGE" ) );

  // PROPERTIES node.
  XmlTree::Node::RefPtr properties ( getImage->child ( "PROPERTIES" ) );

  // ENVELOPE node.
  XmlTree::Node::RefPtr envelope ( properties->child ( "ENVELOPE" ) );

  // Add envelope.  Use Usul convert for full precision.
  envelope->attributes().insert ( Attributes::value_type ( "minx", Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.minimum()[0] ) ) );
  envelope->attributes().insert ( Attributes::value_type ( "miny", Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.minimum()[1] ) ) );
  envelope->attributes().insert ( Attributes::value_type ( "maxx", Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.maximum()[0] ) ) );
  envelope->attributes().insert ( Attributes::value_type ( "maxy", Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.maximum()[1] ) ) );

  // IMAGE_SIZE node.
  Node::RefPtr imageSize ( properties->child ( "IMAGESIZE" ) );

  // Add width and height.
  imageSize->attributes().insert ( Attributes::value_type ( "width", Usul::Convert::Type<unsigned int,std::string>::convert ( width  ) ) );
  imageSize->attributes().insert ( Attributes::value_type ( "height", Usul::Convert::Type<unsigned int,std::string>::convert ( height ) ) );

  // FEATURECOORDSYS node.
  Node::RefPtr featureCoordSys ( properties->child ( "FEATURECOORDSYS" ) );
  featureCoordSys->attributes().insert ( Attributes::value_type ( "id", Usul::Convert::Type<unsigned int,std::string>::convert ( 4326 ) ) );

  // FILTERCOORDSYS node.
  Node::RefPtr filterCoordSys ( properties->child ( "FILTERCOORDSYS" ) );
  filterCoordSys->attributes().insert ( Attributes::value_type ( "id", Usul::Convert::Type<unsigned int,std::string>::convert ( 4326 ) ) );

  // Write the file to an ostringstream.
  std::ostringstream os;
  XmlTree::Writer writer;
  writer.write ( arcXml.get(), os );
  return os.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extension.  Need to figure out how to make requests for other extensions.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayerArcIMS::_cacheFileExtension() const
{
  return "jpg";
}
