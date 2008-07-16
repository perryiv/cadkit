
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to use Yahoo! to geocode data.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Utilities/GeoCode.h"
#include "Minerva/Core/Utilities/Download.h"

#include "XmlTree/Document.h"
#include "XmlTree/XercesLife.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Network/Curl.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Strings/Format.h"

using namespace Minerva::Core::Utilities;


///////////////////////////////////////////////////////////////////////////////
//
//  The request url.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  const std::string requestUrl ( "http://local.yahooapis.com/MapsService/V1/geocode" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GeoCode::GeoCode() :
  _appid( "voundoHV34Hf29AsVm7eelpkHH4NFoxyBSZmk.9dZ5lmX3aWwfiUkrkOgoxRYRTE" )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

GeoCode::~GeoCode()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Yahoo! application id.
//
///////////////////////////////////////////////////////////////////////////////

void GeoCode::applicationId ( const std::string& id )
{
  _appid = id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Yahoo! application id.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& GeoCode::applicationId() const
{
  return _appid;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a lat,lon based on free form location.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec2d GeoCode::operator() ( const std::string& location ) const
{
  // Make a request.
  std::string request ( Usul::Strings::format ( Detail::requestUrl, "?appid=", this->applicationId(), "&location=", Usul::Network::Curl::encode ( location ) ) );
  
  std::string filename;
  if ( Minerva::Core::Utilities::download ( request, filename ) )
  {
    Usul::Scope::RemoveFile remove ( filename );
    
    // Open the xml document.
    XmlTree::XercesLife life;
    XmlTree::Document::RefPtr document ( new XmlTree::Document );
    document->load ( filename );
    
    XmlTree::Node::RefPtr latNode ( document->child ( 0, "Result/Latitude", '/', false ) );
    XmlTree::Node::RefPtr lonNode ( document->child ( 1, "Result/Longitude", '/', false ) );
    
    const double lat ( latNode.valid() ? Usul::Convert::Type<std::string,double>::convert ( latNode->value() ) : 0.0 );
    const double lon ( lonNode.valid() ? Usul::Convert::Type<std::string,double>::convert ( lonNode->value() ) : 0.0 );
    
    std::cout << "Geocoded " << location << " to " << lat << ", " << lon << std::endl;
    
    return Usul::Math::Vec2d ( lon, lat );
  }
  
  return Usul::Math::Vec2d ( 0.0, 0.0 );
}
