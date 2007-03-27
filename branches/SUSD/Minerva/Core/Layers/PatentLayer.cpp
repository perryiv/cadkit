
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/PatentLayer.h"
#include "Minerva/Core/DataObjects/Point.h"

#include "OsgTools/ShapeFactory.h"

#include <algorithm>

using namespace Minerva::Core::Layers;


PatentLayer::PatentLayer() : BaseClass()
{
}


PatentLayer::~PatentLayer()
{
}


void PatentLayer::buildScene( osg::Group* parent )
{
  //BaseClass::buildScene( parent );

  /*TemporalMap &map ( this->_getTemporalMap() );

  OsgTools::ShapeFactory::Ptr sf ( new OsgTools::ShapeFactory );

  for( TemporalMap::iterator i = map.begin(); i != map.end(); ++i )
  {
    osg::ref_ptr< osg::Group > group ( new osg::Group );

    for( DataObjects::const_iterator iter = i->second.begin(); iter != i->second.end(); ++iter )
    {
      Minerva::DataObjects::Point::RefPtr point ( dynamic_cast < Minerva::DataObjects::Point* > ( (*iter).get() ) );
      osg::Vec3 v0 ( point->geometry()->getCenter() );
      v0.z() = static_cast < osg::Vec3::value_type > ( point->size() ) / 100;
      osg::Vec3 v1 ( v0 );

      v0.z()= 0.0;

      osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
      geode->addDrawable( sf->cylinder( 0.025f, 12, v0, v1 ) );

      osg::ref_ptr< osg::Material > material ( new osg::Material );
      
      material->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4( 1.0, 0.0, 0.0, 1.0 ) );

      osg::StateSet *ss = geode->getOrCreateStateSet();
      ss->setAttribute( material.get(), osg::StateAttribute::ON );
      ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );

      group->addChild ( geode.get() );
    }

    parent->addChild( group.get() );
  }*/
}


void PatentLayer::buildDataObjects( Usul::Interfaces::IUnknown *caller )
{
  /*std::string query = "SELECT COUNT(appyear), appyear, geom, id FROM dp_analysis_patents_by_city GROUP BY appyear, geom";
  
  pqxx::result geometryResult ( this->connection()->executeQuery ( query ) );
  
  for ( pqxx::result::const_iterator iter = geometryResult.begin(); iter != geometryResult.end(); ++ iter )
  {
    unsigned int count ( iter[0].as< unsigned int > () );
    unsigned int year ( iter[1].as< unsigned int > () );

    Minerva::DataObjects::Point::RefPtr data ( new Minerva::DataObjects::Point );

    Minerva::postGIS::Geometry::RefPtr geometry ( new Minerva::postGIS::Geometry( this->connection(), 

    data->center ( center );
    data->color ( osg::Vec4( 1.0, 0.0, 0.0, 1.0 ) );
    data->size ( count );
    data->primitiveId ( 2 );
    data->renderBin( this->renderBin() );

    std::ostringstream os;
    os << year << "-1-1";
    OsgTools::Animate::Date date ( os.str() );
    this->_addDate( date, data.get() );
  }*/
}


void PatentLayer::modify ( Usul::Interfaces::IUnknown *caller )
{
}
