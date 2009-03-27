
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Specialized layer for roads.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/RoadLayer.h"
#include "Minerva/Layers/OSM/TileVectorJob.h"
#include "Minerva/Layers/OSM/XAPIMapQuery.h"
#include "Minerva/Layers/OSM/Functions.h"

#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/Line.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Strings/Format.h"
#include "Usul/User/Directory.h"

using namespace Minerva::Layers::OSM;

USUL_FACTORY_REGISTER_CREATOR ( RoadLayer );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RoadLayer::RoadLayer() : BaseClass(),
  _cache ( 0x0 )
{
  const std::string filename ( Usul::Strings::format ( Usul::User::Directory::program ( true ), "road_layer.db" ) );
  CadKit::Database::SQLite::Connection::RefPtr connection ( new CadKit::Database::SQLite::Connection ( filename ) );
  _cache = new Cache ( connection );

  // See http://wiki.openstreetmap.org/wiki/Map_Features for key, value pairs.
  this->addRequest ( 5, Predicate ( "highway", "motorway" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RoadLayer::~RoadLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Job class.
//
///////////////////////////////////////////////////////////////////////////////

class RoadLayerJob : public TileVectorJob
{
public:
  typedef TileVectorJob BaseClass;

  RoadLayerJob ( Cache::RefPtr cache, const std::string& url, const Extents& extents, const Predicate& predicate ) : 
    BaseClass ( cache, url, extents, predicate )
  {
  }

protected:

  virtual ~RoadLayerJob()
  {
  }

  virtual void _started()
  {
    XAPIMapQuery query ( _cache, _url, _predicate, _extents );
  
    // Get the ways.
    Ways ways;
    query.makeWaysQuery ( ways );

    _data.reserve ( ways.size() );

    // Add all the nodes.
    for ( Ways::const_iterator iter = ways.begin(); iter != ways.end(); ++iter )
    {
      OSMWayPtr way ( *iter );
      if ( way.valid() )
      {
        // Make a line.
        Minerva::Core::Data::Line::RefPtr line ( new Minerva::Core::Data::Line );
        Minerva::Core::Data::Line::Vertices vertices;
        vertices.reserve ( way->numNodes() );

        for ( unsigned int i = 0; i < way->numNodes(); ++i )
        {
          OSMNodePtr node ( way->node ( i ) );

          if ( true == node.valid() )
            vertices.push_back ( Usul::Math::Vec3d ( node->location()[0], node->location()[1], 0.0 ) );
        }

        line->line ( vertices );
        line->lineStyle ( Minerva::Core::Data::LineStyle::create ( Usul::Math::Vec4f ( 1.0, 1.0, 0.0, 1.0 ), 2.0 ) );

        DataObject::RefPtr object ( new DataObject );
        object->addGeometry ( line.get() );
        object->name ( Usul::Strings::format ( way->id() ) );

        Usul::Interfaces::IUnknown::QueryPtr unknown ( object );
        _data.push_back ( unknown );
      }
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Launch a job for the predicate.
//
///////////////////////////////////////////////////////////////////////////////

RoadLayer::JobPtr RoadLayer::_launchJob ( 
    const Predicate& predicate, 
    const Extents& extents, 
    unsigned int level, 
    Usul::Jobs::Manager *manager, 
    Usul::Interfaces::IUnknown::RefPtr caller )
{
  return new RoadLayerJob ( _cache, this->url(), extents, predicate );
}
