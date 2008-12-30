
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

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

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

GeoCode::Result GeoCode::operator() ( const std::string& location ) const
{
  // Initialize the result.
  Result answer;

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

    XmlTree::Node::Children results ( document->find ( "Result", false ) );
    if ( true == results.empty() )
      return answer;

    XmlTree::Node::RefPtr result ( results.front() );
    if ( false == result.valid() )
      return answer;

    XmlTree::Node::RefPtr latNode ( result->child ( "Latitude",  false ) );
    if ( false == latNode.valid() )
      return answer;

    XmlTree::Node::RefPtr lonNode ( result->child ( "Longitude", false ) );
    if ( false == lonNode.valid() )
      return answer;

    const double lat ( Usul::Convert::Type<std::string,double>::convert ( latNode->value() ) );
    const double lon ( Usul::Convert::Type<std::string,double>::convert ( lonNode->value() ) );
    answer.location = Usul::Math::Vec2d ( lon, lat );

    XmlTree::Node::RefPtr address ( result->child ( "Address",   false ) );
    XmlTree::Node::RefPtr city    ( result->child ( "City",      false ) );
    XmlTree::Node::RefPtr state   ( result->child ( "State",     false ) );
    XmlTree::Node::RefPtr zip     ( result->child ( "Zip",       false ) );
    XmlTree::Node::RefPtr country ( result->child ( "Country",   false ) );

    answer.address = ( ( true == address.valid() ) ? address->value() : std::string() );
    answer.city    = ( ( true == city.valid()    ) ? city->value()    : std::string() );
    answer.state   = ( ( true == state.valid()   ) ? state->value()   : std::string() );
    answer.zip     = ( ( true == zip.valid()     ) ? zip->value()     : std::string() );
    answer.country = ( ( true == country.valid() ) ? country->value() : std::string() );

    answer.success = true;
  }
  
  return answer;
}
