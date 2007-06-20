
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

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IPointData.h"
#include "Usul/Interfaces/IGeometryCenter.h"
#include "Usul/Interfaces/IProjectCoordinates.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Trace/Trace.h"

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
_quality ( 0.80f ),
_center(),
_autotransform ( true ),
_material ( new osg::Material ),
_group ( new osg::Group )
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
//  Create an auto-transform.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  osg::AutoTransform*   createAutoTransform( const osg::Vec3& center )
  {
    osg::ref_ptr< osg::AutoTransform > autoTransform ( new osg::AutoTransform );

    autoTransform->setPosition ( center );
    autoTransform->setAutoScaleToScreen ( true );

    // Set the normalize state to true, so when the sphere size changes it still looks correct.
    OsgTools::State::StateSet::setNormalize ( autoTransform.get(), true );

    return autoTransform.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Project and convert point.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  void convertToPlanet ( Usul::Math::Vec3d& p )
  {
    Usul::Interfaces::IPlanetCoordinates::QueryPtr  planet  ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IPlanetCoordinates::IID ) );

    if( planet.valid() )
    {
      planet->convertToPlanet( p, p );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene branch for the data object.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::buildScene()
{
  USUL_TRACE_SCOPE;

  if ( this->dirty() )
  {
    // Get the state set
    osg::ref_ptr < osg::StateSet > ss ( _group->getOrCreateStateSet() );

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

    // Set the render bin.
    ss->setRenderBinDetails( this->renderBin(), "RenderBin" );

    // Clear what we have.
    _group->removeChild( 0, _group->getNumChildren() );

    Usul::Interfaces::IPointData::QueryPtr pointData ( this->geometry() );

    Usul::Math::Vec3d center;

    /// Get the center from our data source.
    if( pointData.valid () )
      center = pointData->pointData( );
    
    Detail::convertToPlanet ( center );

    // Convert from Usul's Vec3d to a osg::Vec3
    _center.set( center[0], center[1], center[2] );

    _group->addChild( this->_buildGeometry() );

    // Do we have a label?
    if( this->showLabel() && !this->label().empty() )
      _group->addChild ( this->_buildLabel() );

    this->dirty( false );
  }

  return _group.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the geometry.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildGeometry()
{
  USUL_TRACE_SCOPE;

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
  vertices->push_back ( _center );

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

  unsigned int size ( static_cast < unsigned int > ( 20.0f * this->quality() ) );
  OsgTools::ShapeFactory::MeshSize meshSize ( size, size );
  OsgTools::ShapeFactory::LatitudeRange  latRange  ( 89.9f, -89.9f );
  OsgTools::ShapeFactory::LongitudeRange longRange (  0.0f, 360.0f );

  geode->addDrawable( BaseClass::shapeFactory()->sphere ( _size, meshSize, latRange, longRange  ) );

  osg::ref_ptr< osg::AutoTransform > autoTransform ( Detail::createAutoTransform( _center ) );
  autoTransform->addChild ( geode.get() );

  return autoTransform.release();
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

  float height ( _size );
  float radius ( height * 0.25 );

  // For now use a shape drawable.
  osg::ref_ptr < osg::Cone > cone ( 0x0 );
  
  if( this->autotransform() )
    cone = new osg::Cone( osg::Vec3( 0.0, 0.0, height/2.0 ), radius, height );
  else
    cone = new osg::Cone( osg::Vec3( _center ), radius, height );

  // v1 is also a vector from the center of the sphere, to the point on the sphere.
  osg::Vec3 v1 ( _center );
  v1.normalize();

  // Figure out the needed rotation to place the cone on the earth.
  osg::Quat rotation;
  if( invert )
  {
    rotation.makeRotate( osg::Vec3( 0.0, 0.0, 1.0 ), -v1 );
  }
  else
  {
    rotation.makeRotate( osg::Vec3( 0.0, 0.0, 1.0 ), v1 );
  }

  cone->setRotation( rotation );

  osg::ref_ptr< osg::ShapeDrawable > sd ( new osg::ShapeDrawable( cone.get() ) );

  geode->addDrawable( sd.get() );

  // Use an auto transform if we should.
  if( this->autotransform() )
  {
    osg::ref_ptr < osg::AutoTransform > at ( Detail::createAutoTransform( _center ) );
    at->addChild ( geode.get() );
    return at.release();
  }
  else
  {
    return geode.release();
  }

  // Should never get here.
  return 0x0;
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

  osg::Vec3 v1 ( _center );
  v1.normalize();

  osg::Quat rotation;
  rotation.makeRotate( osg::Vec3( 0.0, 0.0, 1.0 ), v1 );
  cylinder->setRotation( rotation );

  osg::ref_ptr< osg::ShapeDrawable > sd ( new osg::ShapeDrawable( cylinder.get() ) );

  geode->addDrawable ( sd.get() );

  osg::ref_ptr< osg::AutoTransform > autoTransform ( Detail::createAutoTransform( _center ) );
  autoTransform->addChild ( geode.get() );

  return autoTransform.release();
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

  osg::Vec3 v1 ( _center );
  v1.normalize();

  osg::Quat rotation;
  rotation.makeRotate( osg::Vec3( 0.0, 0.0, 1.0 ), v1 );
  osg::Matrix matrix ( rotation );

  osg::ref_ptr< osg::MatrixTransform > mt ( new osg::MatrixTransform );
  mt->setMatrix ( matrix );
  mt->addChild( geode.get() );

  osg::ref_ptr< osg::AutoTransform > autoTransform ( Detail::createAutoTransform( _center ) );
  autoTransform->addChild ( mt.get() );

  return autoTransform.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a cylinder.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildCylinder()
{
  USUL_TRACE_SCOPE;

  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  geode->setUserData( new UserData( this ) );

  if( this->size() > 0 )
  {
    osg::Vec3 v0 ( _center );

    osg::Vec3 v1 ( v0 );
    v1.normalize();
    v1 *= this->size();
    v1 += v0;

    // TODO:
    // Get lat long of center.
    // Offset center by height m.
    // Convert to earth coordinates.

    unsigned int sides ( static_cast < unsigned int > ( 20 * this->quality() ) );
    geode->addDrawable( BaseClass::shapeFactory()->cylinder( 0.00005, sides, v0, v1 ) );
  }

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set quality.
//
///////////////////////////////////////////////////////////////////////////////

void Point::quality( float value )
{
  _quality = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get quality.
//
///////////////////////////////////////////////////////////////////////////////

float Point::quality() const
{
  return _quality;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set use auto transform flag.
//
///////////////////////////////////////////////////////////////////////////////

void Point::autotransform ( bool b )
{
  _autotransform = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get use auto transform flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Point::autotransform () const
{
  return _autotransform;
}
