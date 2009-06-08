
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  WMS layer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include "Minerva/Core/Layers/RasterLayerWms.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Make.h"
#include "Usul/File/Path.h"
#include "Usul/File/Stats.h"
#include "Usul/File/Temp.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Network/WMS.h"
#include "Usul/Registry/Database.h"
#include "Usul/Strings/Format.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "XmlTree/Document.h"
#include "XmlTree/XercesLife.h"

#include "osgDB/ReadFile"

#include "osg/ref_ptr"
#include "osg/Image"

#include <iomanip>
#include <fstream>

using namespace Minerva::Core::Layers;

USUL_FACTORY_REGISTER_CREATOR ( RasterLayerWms );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerWms::RasterLayerWms ( const Extents &maxExtents, const std::string &url, const Options &options ) : 
  BaseClass( maxExtents, url, options )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerWms::RasterLayerWms ( const RasterLayerWms& rhs ) :
  BaseClass ( rhs )
{  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerWms::~RasterLayerWms()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* RasterLayerWms::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr clone ( new RasterLayerWms ( *this ) );
  return clone.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerWms::_download ( const std::string& file, const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *job, IUnknown * )
{
  USUL_TRACE_SCOPE;

  // Get url.
  std::string baseUrl ( this->urlBase() );
  if ( true == baseUrl.empty() )
    return;

  // Get all the options.
  Options options ( this->_options ( extents, width, height, level ) );
  if ( true == options.empty() )
    return;

  // Make wms object.
  Usul::Network::WMS wms ( baseUrl, file, options.begin(), options.end() );

  // Download the file.
  std::ostream *stream ( 0x0 );
  Usul::Interfaces::IUnknown::QueryPtr caller ( job );
  wms.download ( this->timeoutMilliSeconds(), this->maxNumAttempts(), stream, caller.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the full url.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayerWms::urlFull ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level ) const
{
  USUL_TRACE_SCOPE;

  // Get base url.
  std::string baseUrl ( this->urlBase() );

  // Get all the options.
  Options options ( this->_options ( extents, width, height, level ) );

  // Ask the WMS class for the full url.
  const std::string fullUrl ( Usul::Network::WMS::fullUrl ( baseUrl, options ) );
  return fullUrl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all the options.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerWms::Options RasterLayerWms::_options ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level ) const
{
  USUL_TRACE_SCOPE;

  // Get the base options.
  Options options ( this->options() );

  // Add bounding box. Use Usul convert for full precision.
  options[Usul::Network::Names::BBOX] = Usul::Strings::format ( 
    Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.minimum()[0] ), ',', 
    Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.minimum()[1] ), ',', 
    Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.maximum()[0] ), ',', 
    Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.maximum()[1] ) );

  // Add width and height.
  options[Usul::Network::Names::WIDTH]  = Usul::Convert::Type<unsigned int,std::string>::convert ( width  );
  options[Usul::Network::Names::HEIGHT] = Usul::Convert::Type<unsigned int,std::string>::convert ( height );

  return options;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the extents.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  inline RasterLayerWms::Extents parseExtents ( const XmlTree::Node & node, const RasterLayerWms::Extents& defaultExtents )
  {
    typedef XmlTree::Node::Children    Children;
    typedef XmlTree::Node::Attributes  Attributes;
    typedef Usul::Convert::Type<std::string,double> ToDouble;
    
    Children bbNode ( node.find ( "LatLonBoundingBox", false ) );
    if ( false == bbNode.empty() )
    {
      XmlTree::Node::ValidRefPtr bb ( bbNode.front() );
      Attributes& attributes ( bb->attributes() );
      
      return RasterLayerWms::Extents ( ToDouble::convert ( attributes["minx"] ),
                      ToDouble::convert ( attributes["miny"] ),               
                      ToDouble::convert ( attributes["maxx"] ),
                      ToDouble::convert ( attributes["maxy"] ) );
    }
    
    return defaultExtents;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer information for the server.  TODO: Handle styles.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerWms::LayerInfos RasterLayerWms::availableLayers ( const std::string& server )
{
  typedef XmlTree::Node::Children    Children;
  typedef XmlTree::Node::Attributes  Attributes;

  LayerInfos layerInfo;

  const std::string::size_type pos ( server.find_first_of ( '?' ) );
  const std::string prefix ( ( std::string::npos == pos ) ? "?" : "&" );

  // Url request.
  const std::string request ( Usul::Strings::format ( server, prefix, "request=GetCapabilities&service=WMS&version=1.1.1" ) );

  // File to download to.
	std::string name ( Usul::File::Temp::file() );
	Usul::Scope::RemoveFile remove ( name );

	// Download.
	{
    // Get the timeout.
    const unsigned int timeout ( Usul::Registry::Database::instance()["network_download"]["wms_get_capabilities"]["timeout_milliseconds"].get<unsigned int> ( 60000, true ) );
		Usul::Network::Curl curl ( request, name );
		curl.download( timeout, static_cast<std::ostream*> ( 0x0 ), "" );
	}

  // Open the xml document.
	XmlTree::XercesLife life;
  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  document->load ( name );

  // Look for errors first.
  Children exceptions ( document->find ( "ServiceException", true ) );

  if ( false == exceptions.empty() )
  {
    std::string message ( Usul::Strings::format ( "Could not retrieve capabilities using request: ", request ) );
    for ( Children::const_iterator iter = exceptions.begin(); iter != exceptions.end(); ++iter )
    {
      message += Usul::Strings::format ( "  Reason: ", (*iter)->value() );
    }

    throw std::runtime_error ( message.c_str() );
  }

  Children layers ( document->find ( "Layer", true ) );

  if ( layers.size() <= 1 )
    return layerInfo;

  Extents defaultExtents ( Detail::parseExtents ( *layers.front(), Extents ( -180, -90, 180, 90 ) ) );

  for ( Children::const_iterator iter = layers.begin() + 1; iter != layers.end(); ++iter )
  {
    Children nameNode ( (*iter)->find ( "Name", false ) );
    Children titleNode ( (*iter)->find ( "Title", false ) );

    std::string name ( nameNode.empty() ? "" : nameNode.front()->value() );
    std::string title ( titleNode.empty() ? "" : titleNode.front()->value() );

    LayerInfo info;
    info.name = name;
    info.title = title;
    info.extents = Detail::parseExtents ( *(*iter), defaultExtents );

    layerInfo.push_back ( info );
  }

  return layerInfo;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the default options.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerWms::Options RasterLayerWms::defaultOptions()
{
  Options options;

  options[Usul::Network::Names::REQUEST] = "GetMap";
  options[Usul::Network::Names::SRS    ] = "EPSG:4326";
  options[Usul::Network::Names::VERSION] = "1.1.1";
  options["service"] = "WMS";

  return options;
}
