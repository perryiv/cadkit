
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
#include "Minerva/Core/Utilities/Download.h"

#include "XmlTree/XercesLife.h"
#include "XmlTree/Document.h"
#include "XmlTree/Writer.h"

#include "Serialize/XML/TypeWrapper.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Convert/Vector3.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Temp.h"
#include "Usul/Network/Curl.h"
#include "Usul/Network/Names.h"
#include "Usul/Registry/Database.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Strings/Format.h"
#include "Usul/Strings/Split.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include <sstream>

using namespace Minerva::Core::Layers;

USUL_FACTORY_REGISTER_CREATOR ( RasterLayerArcIMS );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Math::Vec3uc );


///////////////////////////////////////////////////////////////////////////////
//
//  Constants used in url when posting data to ArcIMS server.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail {
namespace Contants {

  const std::string SERVICE_NAME ( "ServiceName" );
  const std::string CLIENT_VERSION ( "ClientVersion" );

}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Node names used in ArcXML.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail {
namespace Names {

  const std::string REQUEST ( "REQUEST" );
  const std::string GET_IMAGE ( "GET_IMAGE" );
  const std::string PROPERTIES ( "PROPERTIES" );
  const std::string ENVELOPE ( "ENVELOPE" );

}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerArcIMS::RasterLayerArcIMS() : 
  BaseClass(),
  _type ( "jpg" ),
  _background ( 255, 255, 255 ),
  _transparent ( 255, 255, 255 )
{
  USUL_TRACE_SCOPE;
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerArcIMS::RasterLayerArcIMS ( const RasterLayerArcIMS& rhs ) :
  BaseClass ( rhs ),
  _type ( rhs._type ),
  _background ( 255, 255, 255 ),
  _transparent ( 255, 255, 255 )
{
  USUL_TRACE_SCOPE;
  this->_registerMembers();
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
  USUL_TRACE_SCOPE;
  Usul::Interfaces::IUnknown::QueryPtr clone ( new RasterLayerArcIMS ( *this ) );
  return clone.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members for serialization.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerArcIMS::_registerMembers()
{
  USUL_TRACE_SCOPE;
  this->_addMember ( "type", _type );
  this->_addMember ( "background", _background );
  this->_addMember ( "transparent", _transparent );
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
  std::string url ( this->urlFull ( extents, width, height, level ) );

  // File to download to.
	std::string name ( Usul::File::Temp::file() );
	Usul::Scope::RemoveFile remove ( name );

  // Post the xml.
  {
    Usul::Network::Curl curl ( url, name );

    std::ostream *stream ( 0x0 );
    curl.download ( this->timeoutMilliSeconds(), stream, request );
  }

  XmlTree::Document::RefPtr doc ( 0x0 );

  // Parse the result.
  {
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
      Minerva::Core::Utilities::downloadToFile ( imageUrl, file, this->timeoutMilliSeconds() );
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
  Node::RefPtr request ( arcXml->child ( Detail::Names::REQUEST, true ) );

  // GET_IMAGE node.
  Node::RefPtr getImage ( request->child ( Detail::Names::GET_IMAGE, true ) );

  // PROPERTIES node.
  Node::RefPtr properties ( getImage->child ( Detail::Names::PROPERTIES, true ) );

  // Add envelope.  Use Usul convert for full precision.
  Node::RefPtr envelope ( properties->child ( Detail::Names::ENVELOPE, true ) );
  envelope->attributes().insert ( Attributes::value_type ( "minx", Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.minimum()[0] ) ) );
  envelope->attributes().insert ( Attributes::value_type ( "miny", Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.minimum()[1] ) ) );
  envelope->attributes().insert ( Attributes::value_type ( "maxx", Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.maximum()[0] ) ) );
  envelope->attributes().insert ( Attributes::value_type ( "maxy", Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.maximum()[1] ) ) );

  // Add width and height.
  Node::RefPtr imageSize ( properties->child ( "IMAGESIZE", true ) );
  imageSize->attributes().insert ( Attributes::value_type ( "width", Usul::Convert::Type<unsigned int,std::string>::convert ( width  ) ) );
  imageSize->attributes().insert ( Attributes::value_type ( "height", Usul::Convert::Type<unsigned int,std::string>::convert ( height ) ) );

  // Retrieve answer in WGS 84.
  Node::RefPtr featureCoordSys ( properties->child ( "FEATURECOORDSYS", true ) );
  Node::RefPtr filterCoordSys ( properties->child ( "FILTERCOORDSYS", true ) );
  featureCoordSys->attributes().insert ( Attributes::value_type ( "id", Usul::Convert::Type<unsigned int,std::string>::convert ( 4326 ) ) );
  filterCoordSys->attributes().insert ( Attributes::value_type ( "id", Usul::Convert::Type<unsigned int,std::string>::convert ( 4326 ) ) );

  // Add the format type.
  Node::RefPtr output ( properties->child ( "OUTPUT", true ) );
  output->attributes().insert ( Attributes::value_type ( "type", Usul::Threads::Safe::get ( this->mutex(), _type ) ) );

  Usul::Math::Vec3d bgColor  ( Usul::Threads::Safe::get ( this->mutex(), _background  ) );
  Usul::Math::Vec3d transparent ( Usul::Threads::Safe::get ( this->mutex(), _transparent ) );

  // Add background color.
  Node::RefPtr background ( properties->child ( "BACKGROUND", true ) );
  background->attributes().insert ( Attributes::value_type ( "color",      Usul::Strings::format ( bgColor[0], ",", bgColor[1], ",", bgColor[2] ) ) );
  background->attributes().insert ( Attributes::value_type ( "transcolor", Usul::Strings::format ( transparent[0], ",", transparent[1], ",", transparent[2] ) ) );
  
  // Add the layer list.
  const std::string layers ( this->options()[Usul::Network::Names::LAYERS] );

  if ( false == layers.empty() )
  {
    typedef std::vector<std::string> Strings;
    Strings layerList;
    Usul::Strings::split ( layers, ",", false, layerList );

    Node::RefPtr layerListNode ( properties->child ( "LAYERLIST", true ) );
    for ( Strings::const_iterator iter = layerList.begin(); iter != layerList.end(); ++iter )
    {
      Node::RefPtr layerNode ( new XmlTree::Node ( "LAYERDEF" ) );
      layerNode->attributes()["id"] = *iter;
      layerNode->attributes()["visible"] = "true";
      
      layerListNode->append ( layerNode.get() );
    }
  }

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
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _type;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the default options.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerArcIMS::Options RasterLayerArcIMS::defaultOptions()
{
  return Options();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the extents.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  inline RasterLayerArcIMS::Extents parseEnvelope ( const XmlTree::Node & node, const RasterLayerArcIMS::Extents& defaultExtents )
  {
    typedef XmlTree::Node::Children    Children;
    typedef XmlTree::Node::Attributes  Attributes;
    typedef Usul::Convert::Type<std::string,double> ToDouble;
    
    Children bbNode ( node.find ( Detail::Names::ENVELOPE, false ) );
    if ( false == bbNode.empty() )
    {
      XmlTree::Node::ValidRefPtr bb ( bbNode.front() );
      Attributes& attributes ( bb->attributes() );
      
      return RasterLayerArcIMS::Extents ( ToDouble::convert ( attributes["minx"] ),
                                          ToDouble::convert ( attributes["miny"] ),               
                                          ToDouble::convert ( attributes["maxx"] ),
                                          ToDouble::convert ( attributes["maxy"] ) );
    }
    
    return defaultExtents;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer information for the server.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerArcIMS::LayerInfos RasterLayerArcIMS::availableLayers ( const std::string& url )
{
  typedef XmlTree::Node::Children    Children;
  typedef XmlTree::Node::Attributes  Attributes;
  
  // Xml to request available layers.
  const std::string request ( "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ARCXML version=\"1.1\"><REQUEST><GET_SERVICE_INFO /></REQUEST></ARCXML>" );
  
  // File to download to.
	std::string name ( Usul::File::Temp::file() );
	Usul::Scope::RemoveFile remove ( name );
  
	// Download.
	{
    // Get the timeout.
    const unsigned int timeout ( Usul::Registry::Database::instance()["network_download"]["wms_get_capabilities"]["timeout_milliseconds"].get<unsigned int> ( 60000, true ) );
		Usul::Network::Curl curl ( url, name );
		curl.download( timeout, static_cast<std::ostream*> ( 0x0 ), request );
	}
  
  // Open the xml document.
  XmlTree::XercesLife life;
  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  document->load ( name );
  
  // Get all the layers.
  Children layers ( document->find ( "LAYERINFO", true ) );
  
  if ( layers.empty() )
    return LayerInfos();
  
  // Make the default extents.
  const Extents defaultExtents ( -180, -90, 180, 90 );
  
  LayerInfos layerInfos;
  
  for ( Children::iterator iter = layers.begin() + 1; iter != layers.end(); ++iter )
  {
    Attributes& attributes ( (*iter)->attributes() );

    Children envelopeNode ( (*iter)->find ( Detail::Names::ENVELOPE, false ) );
    
    const std::string name ( attributes["id"] );
    const std::string title ( attributes["name"] ); 
    
    LayerInfo info;
    info.name = name;
    info.title = title;
    info.extents = envelopeNode.empty() ? defaultExtents : Detail::parseEnvelope ( *envelopeNode.front(), defaultExtents );
    
    layerInfos.push_back ( info );
  }
  
  return layerInfos;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a list of available service names.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerArcIMS::ServiceNames RasterLayerArcIMS::availableServices ( const std::string& url )
{
  typedef XmlTree::Node::Children    Children;
  typedef XmlTree::Node::Attributes  Attributes;
  
  // Xml to request available layers.
  const std::string request ( "<?xml version=\"1.0\" encoding=\"UTF-8\"?><GETCLIENTSERVICES />" );
  
  // File to download to.
	std::string name ( Usul::File::Temp::file() );
	Usul::Scope::RemoveFile remove ( name );

  // Get the timeout.
  const unsigned int timeout ( Usul::Registry::Database::instance()["network_download"]["wms_get_capabilities"]["timeout_milliseconds"].get<unsigned int> ( 60000, true ) );
  
	// Download.
	{
    Usul::Network::Curl curl ( url, name );
		curl.download( timeout, static_cast<std::ostream*> ( 0x0 ), request );
	}
  
  // Open the xml document.
	XmlTree::XercesLife life;
  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  document->load ( name );

  // Get all the services.
  Children services ( document->find ( "SERVICE", true ) );

  ServiceNames names;

  for ( Children::const_iterator iter = services.begin(); iter != services.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( (*iter).get() );
    names.push_back ( node->attribute ( "name" ) );
  }

  return names;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the full url.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayerArcIMS::urlFull ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level ) const
{
  // Get base url.
  const std::string baseUrl ( this->urlBase() );

  // Get all the options.
  Options options ( this->options() );

  // Get the service name.
  const std::string serviceName ( options[Detail::Contants::SERVICE_NAME] );

  // We need a service name.
  if ( true == serviceName.empty() )
    throw std::runtime_error ( "Error 3125621866: No service name provided." );

  // Add service name to the base url.
  std::string fullUrl ( Usul::Strings::format ( baseUrl, "?", Detail::Contants::SERVICE_NAME, "=", serviceName ) );

  // Get the client version.
  const std::string clientVersion ( options[Detail::Contants::CLIENT_VERSION] );

  // Add the client version if we have one.
  if ( false == clientVersion.empty() )
    fullUrl = Usul::Strings::format ( fullUrl, "&", Detail::Contants::CLIENT_VERSION, "=", clientVersion );
  
  // Return our full url.
  return fullUrl;
}
