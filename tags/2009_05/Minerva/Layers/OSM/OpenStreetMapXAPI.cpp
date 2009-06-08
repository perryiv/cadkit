
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
#include "Usul/Strings/Split.h"
#include "Usul/User/Directory.h"

using namespace Minerva::Layers::OSM;

///////////////////////////////////////////////////////////////////////////////
//
//  String conversion for Predicate.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{ 
  namespace Convert
  {
    template <> struct Type < Predicate, std::string >
    {
      typedef Type < Predicate, std::string > ThisType;
      static void convert ( Predicate from, std::string &to )
      {
        to = Usul::Strings::format ( from.first, "=", from.second );
      }
      static std::string convert ( Predicate from )
      {
        std::string to;
        ThisType::convert ( from, to );
        return to;
      }
    };
    template <> struct Type < std::string, Predicate >
    {
      typedef Type < std::string, Predicate > ThisType;
      static void convert ( const std::string &from, Predicate &to )
      {
        std::vector<std::string> parts;
        Usul::Strings::split ( from, "=", false, parts );

        if ( 2 == parts.size() )
        {
          to.first = parts[0];
          to.second = parts[1];
        }
      }
      static Predicate convert ( const std::string &from )
      {
        Predicate to;
        ThisType::convert ( from, to );
        return to;
      }
    };
  }
}


SERIALIZE_XML_DECLARE_TYPE_WRAPPER_CONTAINER ( std::vector, Predicate );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenStreetMapXAPI::OpenStreetMapXAPI() : BaseClass(),
  _cache ( 0x0 ),
  _url ( "http://osmxapi.hypercube.telascience.org" ), // Default url.
  _requestMap(),
  _styleMap()
{
  this->extents ( Extents ( -180, -90, 180, 90 ) );

  this->_addMember ( "url", _url );
  this->_addMember ( "request_map", _requestMap );
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
    JobPtr job ( this->_launchJob ( *iter, Extents ( minLon, minLat, maxLon, maxLat ), level, manager, caller ) );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Add a style for a predicate.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMapXAPI::addStyle ( const Predicate& predicate, Style::RefPtr style )
{
  Guard guard ( this );
  _styleMap[predicate] = style;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the style for a predicate.
//
///////////////////////////////////////////////////////////////////////////////

OpenStreetMapXAPI::Style::RefPtr OpenStreetMapXAPI::getStyle ( const Predicate& predicate ) const
{
  {
    Guard guard ( this );
    StyleMap::const_iterator iter ( _styleMap.find ( predicate ) );
    if ( iter != _styleMap.end() )
      return iter->second;
  }

  // Return a default style if we didn't find one.
  Style::RefPtr style ( new Style );

  typedef Minerva::Core::Data::LineStyle LineStyle;
  style->linestyle ( LineStyle::create ( Usul::Math::Vec4f ( 1.0f, 1.0f, 0.0f, 1.0f ), 2.0 ) );
  
  return style;
}
