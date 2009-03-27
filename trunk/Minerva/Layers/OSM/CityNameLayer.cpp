
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Specialized layer for city names.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/CityNameLayer.h"
#include "Minerva/Layers/OSM/TileVectorJob.h"
#include "Minerva/Layers/OSM/XAPIMapQuery.h"
#include "Minerva/Layers/OSM/Functions.h"

#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/Point.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Strings/Format.h"
#include "Usul/User/Directory.h"

using namespace Minerva::Layers::OSM;

USUL_FACTORY_REGISTER_CREATOR ( CityNameLayer );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CityNameLayer::CityNameLayer() : BaseClass(),
  _cache ( 0x0 )
{
  const std::string filename ( Usul::Strings::format ( Usul::User::Directory::program ( true ), "city_names.db" ) );
  CadKit::Database::SQLite::Connection::RefPtr connection ( new CadKit::Database::SQLite::Connection ( filename ) );
  _cache = new Cache ( connection );

  /// Possible place values: http://wiki.openstreetmap.org/wiki/Key:place
  this->addRequest ( 5, Predicate ( "place", "city" ) );
  this->addRequest ( 10, Predicate ( "place", "town" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CityNameLayer::~CityNameLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Job class.
//
///////////////////////////////////////////////////////////////////////////////

class CityNameJob : public TileVectorJob
{
public:
  typedef TileVectorJob BaseClass;

  CityNameJob ( Cache::RefPtr cache, const std::string& url, const Extents& extents, const Predicate& predicate ) : 
    BaseClass ( cache, url, extents, predicate )
  {
  }

protected:

  virtual ~CityNameJob()
  {
  }

  virtual void _started()
  {
    XAPIMapQuery query ( _cache, _url, _predicate, _extents );
  
    // Get modes and ways.
    Nodes nodes;
    query.makeNodesQuery ( nodes );

    // Add all the nodes.
    for ( Nodes::const_iterator iter = nodes.begin(); iter != nodes.end(); ++iter )
    {
      OSMNodePtr node ( *iter );
      if ( node.valid() )
      {
        Usul::Math::Vec3d location (  node->location()[0], node->location()[1], 0.0 );
        Usul::Math::Vec3d labelLocation ( location[0], location[1], 1000.0 );

        Minerva::Core::Data::Point::RefPtr point ( new Minerva::Core::Data::Point );
        point->point ( location );
        point->size ( 10.0 );
        point->autotransform ( false );
        point->color ( Usul::Math::Vec4f ( 1.0, 0.0, 0.0, 1.0 ) );

        DataObject::RefPtr object ( new DataObject );
        object->addGeometry ( point.get() );
        object->name ( Usul::Strings::format ( node->tag<std::string> ( "name" ) ) );
        object->label ( object->name() );
        object->labelPosition ( labelLocation );
        object->labelColor ( DataObject::ColorType ( 1.0, 1.0, 1.0, 1.0 ) );
        object->showLabel ( true );

        Usul::Interfaces::IUnknown::QueryPtr unknown ( object );
        _data.push_back ( unknown );
      }
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Launch the jobs to fetch vector data.
//
///////////////////////////////////////////////////////////////////////////////

CityNameLayer::Jobs CityNameLayer::launchVectorJobs ( 
  double minLon, 
  double minLat, 
  double maxLon, 
  double maxLat, 
  unsigned int level, 
  Usul::Jobs::Manager *manager,
  Usul::Interfaces::IUnknown::RefPtr caller )
{
  return this->_launchVectorJobs<CityNameJob> ( _cache, minLon, minLat, maxLon, maxLat, level, manager, caller );
}
