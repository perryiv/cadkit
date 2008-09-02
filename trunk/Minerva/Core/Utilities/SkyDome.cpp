
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Simple sky dome.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Utilities/SkyDome.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Algorithms/Sphere.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

#include "OsgTools/State/StateSet.h"

#include "osg/CullFace"
#include "osg/Geometry"

using namespace Minerva::Core::Utilities;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SkyDome::SkyDome() : BaseClass(),
  _mutex ( new SkyDome::Mutex ),
  _innerRadius ( 1.0 ),
  _outerRadius ( 2.0 ),
  _dirty ( true ),
  _vertices ( 0x0 ),
  _colors ( 0x0 )
{
  USUL_TRACE_SCOPE;
  
  // Set the state.
  osg::ref_ptr<osg::StateSet> ss ( this->getOrCreateStateSet() );
  
  // Turn on culling.
  ss->setMode ( GL_CULL_FACE, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE );
  
  // Turn on front face culling.
  osg::ref_ptr<osg::CullFace> cf ( new osg::CullFace ( osg::CullFace::FRONT ) );
  ss->setAttributeAndModes ( cf.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE );
  
  // Turn off lighting.
  OsgTools::State::StateSet::setLighting ( ss.get(), false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SkyDome::~SkyDome()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &SkyDome::_destroy ), "3431173664" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy.
//
///////////////////////////////////////////////////////////////////////////////

void SkyDome::_destroy()
{
  USUL_TRACE_SCOPE;
  
  delete _mutex; _mutex = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mutex. Use with caution.
//
///////////////////////////////////////////////////////////////////////////////

SkyDome::Mutex & SkyDome::mutex() const
{
  USUL_TRACE_SCOPE;
  return *_mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the inner radius.
//
///////////////////////////////////////////////////////////////////////////////

void SkyDome::innerRadius ( double value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _innerRadius = value;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the inner radius.
//
///////////////////////////////////////////////////////////////////////////////

double SkyDome::innerRadius() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _innerRadius;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the outer radius.
//
///////////////////////////////////////////////////////////////////////////////

void SkyDome::outerRadius ( double value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _outerRadius = value;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the outer radius.
//
///////////////////////////////////////////////////////////////////////////////

double SkyDome::outerRadius() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _outerRadius;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void SkyDome::dirty ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _dirty = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool SkyDome::dirty() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse.
//
///////////////////////////////////////////////////////////////////////////////

void SkyDome::traverse ( osg::NodeVisitor &nv )
{
  USUL_TRACE_SCOPE;
  
  this->_updateGeometry();

  BaseClass::traverse ( nv );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update geometry.
//
///////////////////////////////////////////////////////////////////////////////

void SkyDome::_updateGeometry()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  // Return now if we aren't dirty.
  if ( false == this->dirty() )
    return;
  
  // Remove what we have.
  this->removeDrawables ( 0, this->getNumDrawables() );
  
  // Radius of sphere.
  const double radius ( this->outerRadius() );
  
  // Define the generator.
  typedef osg::Vec3Array Points;
  typedef osg::Vec3Array Normals;
  typedef std::pair < unsigned int, unsigned int > Strip;
  typedef std::vector < Strip > Strips;
  typedef Usul::Algorithms::LatLongSphere < float > Sphere;
  
  // The points, normals, and indices.
  osg::ref_ptr<Points>  points  ( new Points  );
  osg::ref_ptr<Normals> normals ( new Normals );
  Strips strips;
  
  std::pair<float, float> latRange ( 89.9f, -89.9f );
  std::pair<float, float> longRange (  0.0f, 360.0f );
  
  // Declare the generator and set some properties.
  Sphere sphere;
  sphere.radius         ( radius );
  sphere.normalLength   ( 1.0f );
  sphere.latitudeStart  ( Usul::Math::DEG_TO_RAD * latRange.first );
  sphere.latitudeEnd    ( Usul::Math::DEG_TO_RAD * latRange.second );
  sphere.longitudeStart ( Usul::Math::DEG_TO_RAD * longRange.first );
  sphere.longitudeEnd   ( Usul::Math::DEG_TO_RAD * longRange.second );
  sphere.numLatitude    ( 35 );
  sphere.numLongitude   ( 35 );
  
  // Generate the data.
  sphere ( *points, *normals, strips );
  
  // Make colors.
  osg::ref_ptr<osg::Vec4Array> colors ( new osg::Vec4Array ( points->size() ) );
  std::fill ( colors->begin(), colors->end() , osg::Vec4 ( 0.0, 0.0, 1.0, 1.0 ) );
  
  // Make a new geometry.
  osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );
  geometry->setVertexArray ( points.get() );
  geometry->setNormalArray ( normals.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );
  geometry->setColorArray ( colors.get() );
  geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
  
  // The primitives.
  typedef osg::Geometry::PrimitiveSetList Prims;
  unsigned int numStrips ( strips.size() );
  Prims prims ( numStrips );
  
  // Loop through and set all the primitives.
  for ( unsigned int i = 0; i < numStrips; ++i )
  {
    unsigned int start ( strips.at(i).first );
    unsigned int count ( strips.at(i).second );
    prims.at(i) = new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLE_STRIP, start, count );
  }
  
  // Set the geometry's primitive list.
  geometry->setPrimitiveSetList ( prims );
  
  // Add the geometry.
  this->addDrawable( geometry.get() );
  
  // Don't use display lists.
  geometry->setUseDisplayList ( false );
  
  // Dirty the boudning sphere.
  this->dirtyBound();
  
  // Save these pointers.
  _vertices = points.get();
  _colors = colors.get();
  
  // No longer dirty.
  this->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the colors.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4Array* SkyDome::colors() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _colors.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertices.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3Array* SkyDome::vertices() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _vertices.get();
}
