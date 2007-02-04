
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Decision Theater at Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Magrathea/LatLongGrid.h"
#include "Magrathea/Project.h"

#include "OsgTools/State/StateSet.h"

#include "osg/Vec3"
#include "osg/Geometry"
#include "osg/ref_ptr"
#include "osg/StateSet"
#include "osg/Material"

using namespace Magrathea;

namespace Detail
{
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Build a geometry.
  //
  ///////////////////////////////////////////////////////////////////////////////

  osg::Geometry* buildGeometry ( osg::Vec3Array *vertices, const osg::Vec4& color )
  {
    osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );
    geometry->setVertexArray( vertices );

    geometry->addPrimitiveSet( new osg::DrawArrays( GL_LINE_STRIP, 0, vertices->size() ) );

    osg::ref_ptr< osg::StateSet > ss ( geometry->getOrCreateStateSet() );
    osg::ref_ptr< osg::Material > material ( new osg::Material );
    material->setDiffuse( osg::Material::FRONT_AND_BACK, color );
    ss->setAttribute( material.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );

    return geometry.release();
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Build a latitude band.
  //
  ///////////////////////////////////////////////////////////////////////////////

  osg::Geometry* buildLatBand( double lat, const osg::Vec4& color )
  {
    osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );

    for(double lon = -180.0; lon <= 180; lon += 5 )
    {
      osg::Vec3 vec ( lon, lat, 0.0 );
      Magrathea::projectToEllipsoid( vec, 20000 );
      vertices->push_back( vec );
    }

    return buildGeometry( vertices.get(), color );
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Build a longitude band.
  //
  ///////////////////////////////////////////////////////////////////////////////

  osg::Geometry* buildLongBand( double lon, const osg::Vec4& color )
  {
    osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );

    for(double lat = -90.0; lat <= 90; lat += 5 )
    {
      osg::Vec3 vec ( lon, lat, 0.0 );
      Magrathea::projectToEllipsoid( vec, 20000 );
      vertices->push_back( vec );
    }

    return buildGeometry( vertices.get(), color );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LatLongGrid::LatLongGrid() : BaseClass()
{
  for ( double lat = 10; lat < 90; lat += 10 )
  {
    this->addDrawable( Detail::buildLatBand( lat, osg::Vec4 ( 0.7, 0.7, 1.0, 1.0 ) ) );
  }

  for ( double lat = -10; lat > -90; lat -= 10 )
  {
    this->addDrawable( Detail::buildLatBand( lat, osg::Vec4 ( 0.7, 0.7, 1.0, 1.0 ) ) );
  }

  this->addDrawable( Detail::buildLatBand( 0.0, osg::Vec4 ( 1.0, 1.0, 0.0, 1.0 ) ) );
  this->addDrawable( Detail::buildLatBand( 23.433333333333333, osg::Vec4 ( 1.0, 1.0, 0.0, 1.0 ) ) );
  this->addDrawable( Detail::buildLatBand( 66.56083, osg::Vec4 ( 1.0, 1.0, 0.0, 1.0 ) ) );
  this->addDrawable( Detail::buildLatBand( -23.433333333333333, osg::Vec4 ( 1.0, 1.0, 0.0, 1.0 ) ) );
  this->addDrawable( Detail::buildLatBand( -66.56083, osg::Vec4 ( 1.0, 1.0, 0.0, 1.0 ) ) );

  for ( double lon = 10; lon < 180; lon += 10 )
  {
    this->addDrawable( Detail::buildLongBand( lon, osg::Vec4 ( 0.7, 0.7, 1.0, 1.0 ) ) );
  }

  for ( double lon = -10; lon > -180; lon -= 10 )
  {
    this->addDrawable( Detail::buildLongBand( lon, osg::Vec4 ( 0.7, 0.7, 1.0, 1.0 ) ) );
  }

  this->addDrawable( Detail::buildLongBand( -180.0, osg::Vec4 ( 1.0, 1.0, 0.0, 1.0 ) ) );
  this->addDrawable( Detail::buildLongBand( 0.0, osg::Vec4 ( 1.0, 1.0, 0.0, 1.0 ) ) );

  OsgTools::State::StateSet::setLighting ( this, false );
  OsgTools::State::StateSet::setLineWidth ( this, 1.0f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LatLongGrid::~LatLongGrid()
{
}
