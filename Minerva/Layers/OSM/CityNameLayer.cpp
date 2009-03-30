
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
#include "Minerva/Layers/OSM/Functions.h"
#include "Minerva/Layers/OSM/Parser.h"

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

CityNameLayer::CityNameLayer() : BaseClass()
{
  this->_initializeCache ( "city_names" );

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

  CityNameJob ( Usul::Jobs::Manager* manager, 
    Usul::Jobs::Manager* downloadManager,
    Cache::RefPtr cache, 
    const std::string& url, 
    const Extents& extents, 
    unsigned int level,
    const Predicate& predicate ) : 
    BaseClass ( manager, downloadManager, cache, url, extents, predicate )
  {
    this->priority ( static_cast<int> ( level ) );
  }

protected:

  virtual ~CityNameJob()
  {
  }

  /// Make the request.  Check the cache first.
  void _makeRequest()
  {
    std::string cacheKey ( this->cacheKey() );
    Cache::RefPtr cache ( this->cache() );

    bool isCached ( cache.valid() && cache->hasNodeData ( cacheKey, this->extents() ) );
    if ( isCached )
    {
      Nodes nodes;
      cache->getNodeData ( cacheKey, this->extents(), nodes );
      this->_buildDataObjects ( nodes );

      return;
    }

    this->_startDownload ( this->_buildRequestUrl ( "node" ) );
  }


  /// The downloading has finished.
  void _downloadFinished ( const std::string& filename )
  {
    // Parse modes and ways.
    Nodes nodes;
    Ways ways;
    Parser::parseNodesAndWays ( filename, nodes, ways );

    Cache::RefPtr cache ( this->cache() );
    std::string cacheKey ( this->cacheKey() );
    if ( cache.valid() )
    {
      cache->addNodeData ( cacheKey, this->extents(), nodes );
    }

    this->_buildDataObjects ( nodes );
  }
  
  void _buildDataObjects ( const Nodes& nodes )
  {
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
        this->_addData ( unknown );
      }
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Launch a job for the predicate.
//
///////////////////////////////////////////////////////////////////////////////

CityNameLayer::JobPtr CityNameLayer::_launchJob ( 
    const Predicate& predicate, 
    const Extents& extents, 
    unsigned int level, 
    Usul::Jobs::Manager *manager, 
    Usul::Jobs::Manager *downloadManager, 
    Usul::Interfaces::IUnknown::RefPtr caller )
{
  return new CityNameJob ( manager, downloadManager, this->_getCache(), this->url(), extents, level, predicate );
}
