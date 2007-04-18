
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/RLayer.h"
#include "Minerva/Core/postGIS/Polygon.h"
#include "Minerva/Core/postGIS/Point.h"
#include "Minerva/Core/DataObjects/Point.h"
#include "Minerva/Core/DataObjects/Polygon.h"

#include "OsgTools/ShapeFactory.h"

#include "Serialize/XML/RegisterCreator.h"

#include <algorithm>

using namespace Minerva::Core::Layers;

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

RLayer::RLayer() : BaseClass(),
_mode ( CYLINDER )
{
  this->name( "RLayer" );

  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RLayer::RLayer ( const RLayer& layer ) : BaseClass ( layer ),
_mode ( layer._mode )
{
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members.
//
///////////////////////////////////////////////////////////////////////////////

void RLayer::_registerMembers()
{
  this->_addMember ( "_mode", reinterpret_cast < int & > ( _mode ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone this layer.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* RLayer::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr copy ( new RLayer( *this ) );
  return copy.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

RLayer::~RLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void RLayer::buildScene( osg::Group* parent )
{
  if(this->mode() == CYLINDER)
  {
    this->_buildSceneCylinder( parent );
  }
  else if ( this->mode() == POLYGONS )
  {
    this->_buildScenePolygons( parent );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void RLayer::_buildSceneCylinder( osg::Group* parent )
{
  /*DataObjects &dataObjects ( this->_getDataObjects() );

  OsgTools::ShapeFactory::Ptr sf ( new OsgTools::ShapeFactory );

  for( DataObjects::iterator iter = dataObjects.begin(); iter != dataObjects.end(); ++iter )
  {
    Minerva::Core::DataObjects::Point::RefPtr point ( dynamic_cast < Minerva::Core::DataObjects::Point* > ( (*iter).get() ) );

    Usul::Interfaces::IGeometryCenter::QueryPtr geometryCenter ( (*iter)->geometry() );

    if( geometryCenter.valid () )
    {
      osg::Vec3 v0 ( geometryCenter->geometryCenter() );
      osg::Vec3 v1 ( v0 );

      v0.z()= 0.0;

      osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

      geode->addDrawable( sf->cylinder( 250, 12, v0, v1 ) );

      osg::ref_ptr< osg::Material > material ( new osg::Material );
    
      material->setDiffuse( osg::Material::FRONT_AND_BACK, point->color() );

      osg::StateSet *ss = geode->getOrCreateStateSet();
      ss->setAttribute( material.get(), osg::StateAttribute::ON );
      ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
      ss->setRenderBinDetails( this->renderBin(), "RenderBin" );
      parent->addChild ( geode.get() );
    }
  }*/
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void RLayer::_buildScenePolygons( osg::Group* parent )
{
  BaseClass::buildScene( parent );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void RLayer::buildDataObjects( Usul::Interfaces::IUnknown *caller )
{
  if(this->mode() == CYLINDER)
  {
    this->_buildDataObjectsCylinder( caller );
  }
  else if ( this->mode() == POLYGONS )
  {
    this->_buildDataObjectsPolygons( caller );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void RLayer::_buildDataObjectsCylinder( Usul::Interfaces::IUnknown *caller )
{
  /*std::string query = "SELECT x, y, size FROM " + this->tablename();
  
  pqxx::result geometryResult ( this->connection()->executeQuery ( query ) );

  for ( pqxx::result::const_iterator iter = geometryResult.begin(); iter != geometryResult.end(); ++ iter )
  {
    osg::Vec3 center;
    center.x() = iter[0].as< float > ();
    center.y() = iter[1].as< float > ();

    Minerva::DataObjects::Point::RefPtr data ( new Minerva::DataObjects::Point );

    center.z() = 2000;
    float size ( iter[2].as< float >() );
    center.z() += size * 1000;

    data->center ( center );
    data->color ( (*this->colorFunctor()) ( size ) );
    data->size ( 2.0 );
    data->primitiveId ( 2 );
    data->renderBin( this->renderBin() );

    this->_addDataObject( data.get() );
  }*/
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void RLayer::_buildDataObjectsPolygons( Usul::Interfaces::IUnknown *caller )
{
  std::string query ( "SELECT x,y,size,asBinary(boundary(geom)) FROM " + this->tablename() + ", blkgrp_variables1 WHERE x > x(centroid(blkgrp_variables1.geom))-.1 AND x < x(centroid(blkgrp_variables1.geom))+.1 AND y > y(centroid(blkgrp_variables1.geom))-.1 AND y < y(centroid(blkgrp_variables1.geom))+.1" );

  pqxx::result geometryResult ( this->connection()->executeQuery ( query ) );

  for ( pqxx::result::const_iterator iter = geometryResult.begin(); iter != geometryResult.end(); ++ iter )
  {
    /*osg::Vec4 color ( (*this->colorFunctor())( iter[2].as< double > () ) );

    Minerva::postGIS::Polygon::RefPtr geometry ( new Minerva::postGIS::Polygon ( this->connection(), iter[3] ) );

    Minerva::DataObjects::Boundary::RefPtr data (new Minerva::DataObjects::Boundary );

    data->geometry ( geometry.get() );
    data->color( color );
    data->renderBin( this->renderBin() );

    this->_addDataObject( data.get() );

    data->buildScene();*/
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify the layer.
//
///////////////////////////////////////////////////////////////////////////////

void RLayer::modify( Usul::Interfaces::IUnknown *caller )
{
  this->_getDataObjects().clear();

  this->buildDataObjects( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the mode.
//
///////////////////////////////////////////////////////////////////////////////

void RLayer::mode( Mode m )
{
  _mode = m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the mode.
//
///////////////////////////////////////////////////////////////////////////////

RLayer::Mode RLayer::mode() const
{
  return _mode;
}
