
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to represent point data.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/DataObjects/Point.h"
#include "Minerva/Core/DataObjects/UserData.h"

#include "Usul/Interfaces/IGeometryCenter.h"

#include "OsgTools/State/StateSet.h"
#include "OsgTools/Font.h"

#include "osg/Geometry"
#include "osg/Point"
#include "osg/Geode"
#include "osg/Shape"
#include "osg/ShapeDrawable"
#include "osg/AutoTransform"
#include "osgText/Text"

using namespace Minerva::Core::DataObjects;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Point::Point() :
BaseClass(),
_size ( 1.0 ),
_primitiveId ( 1 ),
_material ( new osg::Material ),
_mt ( new osg::MatrixTransform )
{
  _material->setSpecular ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( 0.4, 0.4, 0.4, 1.0 ) );
  _material->setAmbient ( osg::Material::FRONT_AND_BACK, osg::Vec4( 0.2, 0.2, 0.2, 1.0 ) );
  _material->setShininess( osg::Material::FRONT_AND_BACK, 50 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Point::~Point()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

float Point::size () const
{
  return _size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void Point::size ( float size )
{
  _size = size;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitiveId
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Point::primitiveId () const
{
  return _primitiveId;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the primitiveId.
//
///////////////////////////////////////////////////////////////////////////////

void Point::primitiveId ( unsigned int primitiveId )
{
  _primitiveId = primitiveId;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene branch for the data object.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::buildScene()
{
  if ( this->dirty() )
  {
    // Get the state set
    osg::ref_ptr < osg::StateSet > ss ( _mt->getOrCreateStateSet() );

    // Set the material's diffuse color
    _material->setDiffuse ( osg::Material::FRONT_AND_BACK, this->color() );

    // Set proper state for transparency
    if( 1.0f == this->color().w() )
    {
      ss->setMode ( GL_BLEND,      osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE );
      ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::ON  | osg::StateAttribute::OVERRIDE );
    }
    else
    {
      _material->setTransparency( osg::Material::FRONT_AND_BACK, this->color().w() );
      ss->setMode ( GL_BLEND,      osg::StateAttribute::ON  | osg::StateAttribute::OVERRIDE );
      ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE );
    }

    // Set the material.
    ss->setAttribute ( _material.get(), osg::StateAttribute::ON );

    _mt->removeChild( 0, _mt->getNumChildren() );

    osg::ref_ptr< osg::AutoTransform > autoTransform ( new osg::AutoTransform );

    Usul::Interfaces::IGeometryCenter::QueryPtr geometryCenter ( this->geometry() );

    if( geometryCenter.valid () )
      autoTransform->setPosition ( geometryCenter->geometryCenter() );

    autoTransform->setAutoScaleToScreen ( true );

    // Set the normalize state to true, so when the sphere size changes it still looks correct.
    OsgTools::State::StateSet::setNormalize ( _mt.get(), true );

    //_mt->setMatrix ( osg::Matrix::translate( _center ) );

    ss->setRenderBinDetails( this->renderBin(), "RenderBin" );

    autoTransform->addChild ( this->_buildGeometry() );

    _mt->addChild( autoTransform.get() );

    // Do we have a label?
    if( this->showLabel() && !this->label().empty() )
      _mt->addChild ( this->_buildLabel() );

    this->dirty( false );
  }

  return _mt.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the geometry.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildGeometry()
{
  switch ( _primitiveId )
  {
  case POINT:          return this->_buildPoint();
  case SPHERE:         return this->_buildSphere();
  case CONE:           return this->_buildCone( false );
  case DISK:           return this->_buildDisk();
  case CUBE:           return this->_buildCube();
  case INVERTED_CONE:  return this->_buildCone( true );
  case CYLINDER:       return this->_buildCylinder();
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a point.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildPoint()
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  geode->setUserData( new UserData( this ) );
  
  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );

  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  vertices->push_back ( osg::Vec3 ( 0.0, 0.0, 0.0 ) );

  geometry->setVertexArray( vertices.get() );

  osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );
  colors->push_back( this->color() );

  geometry->setColorArray ( colors.get() );
  geometry->setColorBinding ( osg::Geometry::BIND_OVERALL );

  osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array );
  normals->push_back( osg::Vec3( 0.0f,-1.0f,0.0f ) );
  geometry->setNormalArray( normals.get() );
  geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );

  osg::ref_ptr < osg::Point > ps ( new osg::Point );
  ps->setSize( _size );
	
  osg::ref_ptr < osg::StateSet > ss ( geometry->getOrCreateStateSet() );
  ss->setAttributeAndModes( ps.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );

  // Turn off lighting.
  OsgTools::State::StateSet::setLighting ( ss.get(), false );

  geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::POINTS, 0, vertices->size() ) );

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a sphere.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildSphere()
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  geode->setUserData( new UserData( this ) );

  OsgTools::ShapeFactory::MeshSize meshSize ( 5, 5 );
  OsgTools::ShapeFactory::LatitudeRange  latRange  ( 89.9f, -89.9f );
  OsgTools::ShapeFactory::LongitudeRange longRange (  0.0f, 360.0f );

  geode->addDrawable( BaseClass::shapeFactory()->sphere ( _size, meshSize, latRange, longRange  ) );

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a cone.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildCone( bool invert )
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  geode->setUserData( new UserData( this ) );

  // For now use a shape drawable.
  osg::ref_ptr < osg::Cone > cone ( new osg::Cone( osg::Vec3( 0.0, 0.0, 0.0 ), _size * 0.25, _size ) );
  osg::ref_ptr< osg::ShapeDrawable > sd ( new osg::ShapeDrawable( cone.get() ) );

  geode->addDrawable( sd.get() );

  if( invert )
  {
    osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );
    mt->setMatrix ( osg::Matrix::rotate ( osg::PI, osg::Vec3 ( 1.0, 0.0, 0.0 ) ) );

    mt->addChild( geode.get() );

    return mt.release();
  }

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a disk.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildDisk()
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  geode->setUserData( new UserData( this ) );

  // For now use a short cylinder.  Ellipsoid would be better.
  osg::ref_ptr < osg::Cylinder > cylinder ( new osg::Cylinder( osg::Vec3( 0.0, 0.0, 0.0 ), _size, _size * 0.25 ) );
  osg::ref_ptr< osg::ShapeDrawable > sd ( new osg::ShapeDrawable( cylinder.get() ) );

  geode->addDrawable ( sd.get() );

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a cube.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildCube()
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  geode->setUserData( new UserData( this ) );

  geode->addDrawable( BaseClass::shapeFactory()->cube ( osg::Vec3 ( _size, _size, _size )  ) );

  return geode.release();
}


osg::Node* Point::_buildCylinder()
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  geode->setUserData( new UserData( this ) );

  if( this->size() > 0 )
  {
    osg::Vec3 v0 ( 0.0, 0.0, 0.0 );
    osg::Vec3 v1 ( v0 );
    v0.z() += this->size();

    geode->addDrawable( BaseClass::shapeFactory()->cylinder( 5.0f, 12, v0, v1 ) );
  }

  return geode.release();
}
