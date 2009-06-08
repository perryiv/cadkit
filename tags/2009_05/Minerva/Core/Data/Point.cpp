
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Data/Point.h"

#include "OsgTools/Callbacks/SortBackToFront.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Font.h"
#include "OsgTools/Ray.h"
#include "OsgTools/Convert.h"

#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "osg/Geometry"
#include "osg/Point"
#include "osg/Geode"
#include "osg/Material"
#include "osg/Shape"
#include "osg/ShapeDrawable"
#include "osg/AutoTransform"
#include "osgText/Text"

using namespace Minerva::Core::Data;

USUL_IMPLEMENT_TYPE_ID ( Point );


///////////////////////////////////////////////////////////////////////////////
//
//  Static Member.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::ShapeFactory::Ptr Point::_sf ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Return the shape factory.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::ShapeFactory* Point::shapeFactory()
{
  if ( 0x0 == _sf.get() )
    _sf = new OsgTools::ShapeFactory;
  return _sf.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Point::Point() : BaseClass(),
  _point(),
  _size ( 1.0 ),
  _secondarySize ( 1.0 ),
  _primitiveId ( 1 ),
  _quality ( 0.80f ),
  _autotransform ( true ),
  _color ( 0.0, 0.0, 0.0, 1.0 ),
  _transform ( 0x0 )
{
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
//  Return the point
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec3d Point::pointData() const
{
  Usul::Math::Vec3d point ( this->point() );
  Usul::Math::Vec3d offset ( this->spatialOffset() );
  
  point.set( point[0] + offset[0], point[1] + offset[1], point[2] + offset[2] );

  return point;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the point.
//
///////////////////////////////////////////////////////////////////////////////

void Point::point ( const Usul::Math::Vec3d& p )
{
  Guard guard ( this->mutex() );
  _point = p;
  
  // Set the extents.
  this->extents ( Extents ( Extents::Vertex ( p[0], p[1] ), Extents::Vertex ( p[0], p[1] ) ) );
  
  // Dirty.
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the point.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Math::Vec3d Point::point() const
{
  Guard guard ( this->mutex() );
  return _point;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

float Point::size() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void Point::size ( float size )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _size = size;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the secondary size.
//
///////////////////////////////////////////////////////////////////////////////

float Point::secondarySize() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _secondarySize;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the secondary size.
//
///////////////////////////////////////////////////////////////////////////////

void Point::secondarySize ( float size )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _secondarySize = size;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitiveId
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Point::primitiveId() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _primitiveId;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the primitiveId.
//
///////////////////////////////////////////////////////////////////////////////

void Point::primitiveId ( unsigned int primitiveId )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
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
  osg::AutoTransform*   createAutoTransform( const osg::Vec3d& center )
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
  void convertToPlanet ( Usul::Math::Vec3d& p, Usul::Interfaces::IUnknown* caller )
  {
    Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );
    
    if( planet.valid() )
    {
      planet->convertToPlanet( Usul::Math::Vec3d ( p ), p );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene branch for the data object.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildScene ( Usul::Interfaces::IUnknown * caller )
{
  USUL_TRACE_SCOPE;
  
  /// Get the center. from our data source.
  Usul::Math::Vec3d center ( this->pointData() );

  // Set the height.
  Usul::Interfaces::IElevationDatabase::QueryPtr elevation ( caller );
  center[2] = this->_elevation ( center, elevation );
  
  const double height ( elevation.valid() ? elevation->elevationAtLatLong ( center[1], center[0] ) : 0.0 );
  
  return this->_buildScene ( center, height, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene branch for the data object.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildScene ( const Vec3d& point, double height, Usul::Interfaces::IUnknown * caller )
{
  USUL_TRACE_SCOPE;
  
  // Make the group.
  osg::ref_ptr < osg::Group > group ( new osg::Group );

  // Save the center in lat/lon coordinates.
  Vec3d location ( point [ 0 ], point [ 1 ], point [ 2 ] );
  
  // Convert to planet coordinates.
  Detail::convertToPlanet ( location, caller );
  
  // Location on earth in cartesian coordinates.
  osg::Vec3d earthLocation ( location[0], location[1], location[2] );
  
  osg::ref_ptr < osg::Node > geometry ( this->_buildGeometry ( earthLocation, caller ) );
  
  // Get the state set
  osg::ref_ptr < osg::StateSet > ss ( geometry->getOrCreateStateSet() );
  
  // Set the render bin.
  ss->setRenderBinDetails ( this->renderBin(), "RenderBin" );
  
  // Set the material.
  osg::Vec4f color ( Usul::Convert::Type<Color,osg::Vec4f>::convert ( this->color() ) );
  OsgTools::State::StateSet::setMaterial ( group.get(), color, color, color.w() );
  
  // Make sure lighting is on.
  OsgTools::State::StateSet::setLighting ( ss.get(), true );
  
  // Add the geometry to our group.
  group->addChild ( geometry.get () );
  
  if ( this->extrude() )
  {
    Usul::Math::Vec3d p ( point[0], point[1], height );
    Detail::convertToPlanet ( p, caller );
    
    OsgTools::Ray ray;
    ray.thickness ( 1 );
    ray.color ( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
    ray.start ( osg::Vec3 ( p[0], p[1], p[2] ) );
    ray.end ( earthLocation );
    
    group->addChild ( ray() );
  }
  
  this->dirty ( false );
  
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the geometry.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildGeometry( const osg::Vec3d& earthLocation, Usul::Interfaces::IUnknown* caller )
{
  USUL_TRACE_SCOPE;
  
  // Redirect to proper build function.
  switch ( this->primitiveId() )
  {
    case POINT:          return this->_buildPoint ( earthLocation );
    case SPHERE:         return this->_buildSphere ( earthLocation );
    case CONE:           return this->_buildCone ( earthLocation, false );
    case DISK:           return this->_buildDisk ( earthLocation );
    case CUBE:           return this->_buildCube ( earthLocation );
    case INVERTED_CONE:  return this->_buildCone ( earthLocation, true );
    case CYLINDER:       return this->_buildCylinder ( earthLocation, caller );
  }
  
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a point.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildPoint ( const osg::Vec3d& earthLocation )
{
  USUL_TRACE_SCOPE;
  
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  
  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
  
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  vertices->push_back ( osg::Vec3 ( 0.0, 0.0, 0.0 ) );
  
  geometry->setVertexArray( vertices.get() );
  
  osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );
  colors->push_back( Usul::Convert::Type<Color,osg::Vec4f>::convert ( this->color() ) );
  
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
  
  geode->addDrawable ( geometry.get() );

  osg::ref_ptr<MatrixTransform> mt ( new MatrixTransform );
  mt->setMatrix ( osg::Matrix::translate ( earthLocation ) );
  mt->addChild ( geode.get() );
  
  Usul::Threads::Safe::set ( this->mutex(), mt.get(), _transform );
  
  return mt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a sphere.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildSphere ( const osg::Vec3d& earthLocation )
{
  USUL_TRACE_SCOPE;

  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

  unsigned int size ( static_cast < unsigned int > ( 20.0f * this->quality() ) );
  OsgTools::ShapeFactory::MeshSize meshSize ( size, size );
  OsgTools::ShapeFactory::LatitudeRange  latRange  ( 89.9f, -89.9f );
  OsgTools::ShapeFactory::LongitudeRange longRange (  0.0f, 360.0f );

  osg::ref_ptr < osg::Geometry> geometry ( Point::shapeFactory()->sphere ( this->size(), meshSize, latRange, longRange ) );
  geometry->setUseDisplayList ( false );
  geometry->setUseVertexBufferObjects ( true );
  geode->addDrawable( geometry.get() );

  if ( this->autotransform() )
  {
    osg::ref_ptr< osg::AutoTransform > autoTransform ( Detail::createAutoTransform ( earthLocation ) );
    autoTransform->addChild ( geode.get() );
    return autoTransform.release();
  }

  osg::ref_ptr<MatrixTransform> mt ( new MatrixTransform );
  mt->setMatrix ( osg::Matrix::translate ( earthLocation ) );
  mt->addChild ( geode.get() );
  
  Usul::Threads::Safe::set ( this->mutex(), mt.get(), _transform );
  
  return mt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a cone.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildCone ( const osg::Vec3d& earthLocation, bool invert )
{
  USUL_TRACE_SCOPE;
  
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  
  float height ( _size );
  float radius ( height * 0.25 );
  
  // For now use a shape drawable.
  osg::ref_ptr < osg::Cone > cone ( 0x0 );
  
  if( this->autotransform() )
    cone = new osg::Cone ( osg::Vec3 ( 0.0, 0.0, height/2.0 ), radius, height );
  else
    cone = new osg::Cone ( osg::Vec3 ( earthLocation ), radius, height );
  
  // v1 is also a vector from the center of the sphere, to the point on the sphere.
  osg::Vec3 v1 ( earthLocation );
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
    osg::ref_ptr < osg::AutoTransform > at ( Detail::createAutoTransform( earthLocation ) );
    at->addChild ( geode.get() );
    return at.release();
  }
  else
  {
    osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );
    //mt->setMatrix ( osg::Matrix::translate( v1 * ( height / 2.0 ) ) );
    mt->addChild ( geode.get() );
    return mt.release();
  }
  
  // Should never get here.
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a disk.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildDisk ( const osg::Vec3d& earthLocation )
{
  USUL_TRACE_SCOPE;
  
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  
  // For now use a short cylinder.  Ellipsoid would be better.
  osg::ref_ptr < osg::Cylinder > cylinder ( new osg::Cylinder( osg::Vec3( 0.0, 0.0, 0.0 ), _size, _size * 0.25 ) );
  
  osg::Vec3 v1 ( earthLocation );
  v1.normalize();
  
  osg::Quat rotation;
  rotation.makeRotate( osg::Vec3( 0.0, 0.0, 1.0 ), v1 );
  cylinder->setRotation( rotation );
  
  osg::ref_ptr< osg::ShapeDrawable > sd ( new osg::ShapeDrawable( cylinder.get() ) );
  
  geode->addDrawable ( sd.get() );
  
  osg::ref_ptr< osg::AutoTransform > autoTransform ( Detail::createAutoTransform( earthLocation ) );
  autoTransform->addChild ( geode.get() );
  
  return autoTransform.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a cube.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildCube ( const osg::Vec3d& earthLocation )
{
  USUL_TRACE_SCOPE;
  
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  
  geode->addDrawable( Point::shapeFactory()->cube ( osg::Vec3 ( _size, _size, _size )  ) );
  
  osg::Vec3 v1 ( earthLocation );
  v1.normalize();
  
  osg::Quat rotation;
  rotation.makeRotate( osg::Vec3( 0.0, 0.0, 1.0 ), v1 );
  osg::Matrix matrix ( rotation );
  
  osg::ref_ptr< osg::MatrixTransform > mt ( new osg::MatrixTransform );
  mt->setMatrix ( matrix );
  mt->addChild( geode.get() );
  
  osg::ref_ptr< osg::AutoTransform > autoTransform ( Detail::createAutoTransform( earthLocation ) );
  autoTransform->addChild ( mt.get() );
  
  return autoTransform.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a cylinder.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::_buildCylinder ( const osg::Vec3d& earthLocation, Usul::Interfaces::IUnknown * caller )
{
  USUL_TRACE_SCOPE;

  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

  if( this->size() > 0 )
  {
    osg::Vec3 v0 ( earthLocation );

    osg::Vec3 v1 ( v0 );

    Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );

    if( planet.valid() )
    {
      Usul::Math::Vec3d latLon;
      planet->convertFromPlanet( Usul::Math::Vec3d ( v0[0], v0[1], v0[2] ), latLon );
      latLon[2] += this->size() * 1000;
      Usul::Math::Vec3d earth;
      planet->convertToPlanet ( latLon, earth );
      v1.set ( earth[0], earth[1], earth[2] );
    }

    unsigned int sides ( static_cast < unsigned int > ( 20 * this->quality() ) );
    osg::ref_ptr < osg::Geometry > geometry ( Point::shapeFactory()->cylinder( this->secondarySize()* 1000, sides, v0, v1, !this->isSemiTransparent() ) );
    geode->addDrawable( geometry.get() );

    if( this->isSemiTransparent() )
    {
      geode->setCullCallback ( new OsgTools::Callbacks::SortBackToFront );
      geometry->setUseDisplayList ( false );
    }
  }

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set quality.
//
///////////////////////////////////////////////////////////////////////////////

void Point::quality ( float value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _quality = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get quality.
//
///////////////////////////////////////////////////////////////////////////////

float Point::quality() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _quality;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set use auto transform flag.
//
///////////////////////////////////////////////////////////////////////////////

void Point::autotransform ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _autotransform = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get use auto transform flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Point::autotransform () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _autotransform;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec4f Point::color() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

void Point::color ( const Color& color )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  if ( false == color.equal ( _color ) )
  {
    // Set the internal color.
    _color = color;
    this->dirty( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this geometry transparent?
//
///////////////////////////////////////////////////////////////////////////////

bool Point::isSemiTransparent() const
{
  USUL_TRACE_SCOPE;
  return 1.0 != this->color()[3];
}
