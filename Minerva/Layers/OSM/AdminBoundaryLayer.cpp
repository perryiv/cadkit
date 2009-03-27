
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

#include "Minerva/Layers/OSM/AdminBoundaryLayer.h"
#include "Minerva/Layers/OSM/TileVectorJob.h"
#include "Minerva/Layers/OSM/XAPIMapQuery.h"
#include "Minerva/Layers/OSM/Functions.h"

#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/Line.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Strings/Format.h"
#include "Usul/User/Directory.h"

using namespace Minerva::Layers::OSM;

USUL_FACTORY_REGISTER_CREATOR ( AdminBoundaryLayer );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AdminBoundaryLayer::AdminBoundaryLayer() : BaseClass(),
  _cache ( 0x0 )
{
  const std::string filename ( Usul::Strings::format ( Usul::User::Directory::program ( true ), "admin_boundaries.db" ) );
  CadKit::Database::SQLite::Connection::RefPtr connection ( new CadKit::Database::SQLite::Connection ( filename ) );
  _cache = new Cache ( connection );

  /// Possible place values: http://wiki.openstreetmap.org/wiki/Key:boundary
  this->addRequest ( 0, Predicate ( "boundary", "administrative" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AdminBoundaryLayer::~AdminBoundaryLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Job class.
//
///////////////////////////////////////////////////////////////////////////////

class AdminBoundaryJob : public TileVectorJob
{
public:
  typedef TileVectorJob BaseClass;

  AdminBoundaryJob ( Cache::RefPtr cache, const std::string& url, const Extents& extents, const Predicate& predicate ) : 
    BaseClass ( cache, url, extents, predicate )
  {
  }

protected:

  virtual ~AdminBoundaryJob()
  {
  }

  virtual void _started()
  {
    XAPIMapQuery query ( _cache, _url, _predicate, _extents );
  
    // Get nodes and ways.
    Nodes nodes;
    Ways ways;
    query.makeNodesAndWaysQuery ( nodes, ways );

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
      }
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Launch a job for the predicate.
//
///////////////////////////////////////////////////////////////////////////////

AdminBoundaryLayer::JobPtr AdminBoundaryLayer::_launchJob ( 
    const Predicate& predicate, 
    const Extents& extents, 
    unsigned int level, 
    Usul::Jobs::Manager *manager, 
    Usul::Interfaces::IUnknown::RefPtr caller )
{
  return new AdminBoundaryJob ( _cache, this->url(), extents, predicate );
}
