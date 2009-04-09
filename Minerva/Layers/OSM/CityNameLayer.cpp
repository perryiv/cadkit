
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

USUL_FACTORY_REGISTER_CREATOR_WITH_NAME ( "OSM:CityNameLayer", CityNameLayer );

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
    Cache::RefPtr cache, 
    const std::string& url, 
    const Extents& extents, 
    unsigned int level,
    const Predicate& predicate ) : 
    BaseClass ( manager, cache, url, extents, level, predicate )
  {
    this->priority ( static_cast<int> ( level ) );
  }

protected:

  virtual ~CityNameJob()
  {
  }

  /// Make the request.  Check the cache first.
  void _started()
  {
    this->_setStatus ( "Started" );

    XAPIMapQuery query ( this->_makeQuery() );

    Nodes nodes;
    query.makeNodesQuery ( nodes, this );

    this->_setStatus ( "Building data objects" );

    this->_buildDataObjects ( nodes );

    this->_setStatus ( "Finished" );
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
    Usul::Interfaces::IUnknown::RefPtr caller )
{
  return new CityNameJob ( manager, this->_getCache(), this->url(), extents, level, predicate );
}
