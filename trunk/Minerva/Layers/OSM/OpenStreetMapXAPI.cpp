
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OpenStreetMap layer that uses the extended API (XAPI). 
//  See http://wiki.openstreetmap.org/index.php/Osmxapi
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/OpenStreetMapXAPI.h"
#include "Minerva/Layers/OSM/TileVectorJob.h"

#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/Line.h"
#include "Minerva/Core/Data/Point.h"
#include "Minerva/Core/Data/MultiPoint.h"
#include "Minerva/Core/Data/TimeStamp.h"
#include "Minerva/Core/Factory/Readers.h"

#include "XmlTree/Document.h"
#include "XmlTree/XercesLife.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Threads/Safe.h"

#include "boost/foreach.hpp"

using namespace Minerva::Layers::OSM;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenStreetMapXAPI::OpenStreetMapXAPI() : BaseClass(),
  _url ( "http://osmxapi.hypercube.telascience.org" ), // Default url.
  _requestMap()
{
  this->extents ( Extents ( -180, -90, 180, 90 ) );

  // Populate the requests to make.
  // See http://wiki.openstreetmap.org/wiki/Map_Features for key, value pairs.
  //_requestMap[5].push_back ( Predicate ( "highway", "motorway" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenStreetMapXAPI::~OpenStreetMapXAPI()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Serialize.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMapXAPI::serialize ( XmlTree::Node &parent ) const
{
  Serialize::XML::DataMemberMap dataMemberMap ( Usul::Threads::Safe::get ( this->mutex(), _dataMemberMap ) );
  
  // Don't serialize the layers.
  dataMemberMap.erase ( "layers" );
  
  // Serialize.
  dataMemberMap.serialize ( parent );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all the predicates to use at a given level.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMapXAPI::_getAllPredicates ( unsigned int level, Predicates& predicates ) const
{
  Guard guard ( this );

  // Find all predicates for levels greater than or equal to the level.
  RequestMap::const_iterator iterator ( _requestMap.begin() );
  while ( iterator != _requestMap.end() )
  {
    if ( level >= iterator->first )
    {
      const Predicates& predicatesForLevel ( iterator->second );
      std::copy ( predicatesForLevel.begin(), predicatesForLevel.end(), std::back_inserter ( predicates ) );
    }

    ++iterator;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the url.
//
///////////////////////////////////////////////////////////////////////////////

std::string OpenStreetMapXAPI::url() const
{
  Guard guard ( this );
  return _url;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a predicate request.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMapXAPI::addRequest ( unsigned int level, const Predicate& predicate )
{
  _requestMap[level].push_back ( predicate );
}
