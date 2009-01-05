
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


USUL_FACTORY_REGISTER_CREATOR ( OpenStreetMapXAPI );


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
  _requestMap[5].push_back ( Predicate ( "highway", "motorway" ) );
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
//  Launch the jobs to fetch vector data.
//
///////////////////////////////////////////////////////////////////////////////

OpenStreetMapXAPI::Jobs OpenStreetMapXAPI::launchVectorJobs ( 
  double minLon, 
  double minLat, 
  double maxLon, 
  double maxLat, 
  unsigned int level, 
  Usul::Jobs::Manager *manager,
  Usul::Interfaces::IUnknown *caller )
{
  // Return now if there is no manager.
  if ( 0x0 == manager )
    return Jobs();

  // Get all the predicates.
  Predicates predicates;
  this->_getAllPredicates ( level, predicates );

  Jobs jobs;

  // Create a job for each predicate.
  for ( Predicates::const_iterator iter = predicates.begin(); iter != predicates.end(); ++iter )
  {
    TileVectorJob::RefPtr job ( new TileVectorJob ( _url, Extents ( minLon, minLat, maxLon, maxLat ), *iter ) );
    jobs.push_back ( Usul::Interfaces::IUnknown::QueryPtr ( job ) );

    manager->addJob ( job.get() );
  }

  return jobs;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all the predicates to use at a given level.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMapXAPI::_getAllPredicates ( unsigned int level, Predicates& predicates ) const
{
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
