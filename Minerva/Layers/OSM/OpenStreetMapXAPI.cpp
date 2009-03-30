
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

#include "Usul/Jobs/Manager.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Strings/Format.h"
#include "Usul/User/Directory.h"

using namespace Minerva::Layers::OSM;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenStreetMapXAPI::OpenStreetMapXAPI() : BaseClass(),
  _cache ( 0x0 ),
  _url ( "http://osmxapi.hypercube.telascience.org" ), // Default url.
  _requestMap(),
  _downloadManager ( new Usul::Jobs::Manager ( "OSM Download Manager", 1, true ) )
{
  this->extents ( Extents ( -180, -90, 180, 90 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenStreetMapXAPI::~OpenStreetMapXAPI()
{
  delete _downloadManager;
  _downloadManager = 0x0;
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
  Usul::Interfaces::IUnknown::RefPtr caller )
{
  // Return now if there is no manager.
  if ( 0x0 == manager )
    return Jobs();

  // Don't make any requests if we aren't visible.
  if ( false == this->visibility() )
    return Jobs();

  // Get all the predicates.
  Predicates predicates;
  this->_getAllPredicates ( level, predicates );

  Jobs jobs;

  // Create a job for each predicate.
  for ( Predicates::const_iterator iter = predicates.begin(); iter != predicates.end(); ++iter )
  {
    JobPtr job ( this->_launchJob ( *iter, Extents ( minLon, minLat, maxLon, maxLat ), level, manager, _downloadManager, caller ) );
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
  Guard guard ( this );
  _requestMap[level].push_back ( predicate );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the cache.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMapXAPI::_initializeCache ( const std::string& name )
{
  const std::string filename ( Usul::Strings::format ( Usul::User::Directory::program ( true ), name, ".db" ) );
  CadKit::Database::SQLite::Connection::RefPtr connection ( new CadKit::Database::SQLite::Connection ( filename ) );

  Guard guard ( this );
  _cache = new Cache ( connection );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the cache.
//
///////////////////////////////////////////////////////////////////////////////

Cache::RefPtr OpenStreetMapXAPI::_getCache() const
{
  Guard guard ( this );
  return _cache;
}