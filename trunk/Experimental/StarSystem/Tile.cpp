
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A recursive tile.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include "StarSystem/Tile.h"
#include "StarSystem/Body.h"
#include "StarSystem/RasterLayer.h"

#include "OsgTools/Mesh.h"
#include "OsgTools/State/StateSet.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Random.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Trace/Trace.h"

#include "ossim/base/ossimEllipsoid.h"

#include "osgUtil/CullVisitor"

#include "osg/Geode"
#include "osg/Material"
#include "osg/Texture2D"

using namespace StarSystem;

USUL_IMPLEMENT_TYPE_ID ( Tile );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Tile::Tile ( unsigned int level, const osg::Vec2d &mn, const osg::Vec2d &mx, 
             unsigned int numRows, unsigned int numColumns, double splitDistance, 
             Body *body, RasterLayer* raster ) : BaseClass(),
  _mutex ( new Tile::Mutex ),
  _body ( body ),
  _min ( mn ),
  _max ( mx ),
  _splitDistance ( splitDistance ),
  _mesh ( new OsgTools::Mesh ( numRows, numColumns ) ),
  _level ( level ),
  _dirty ( true ),
  _raster ( raster ),
  _children ()
{
  USUL_TRACE_SCOPE;

  Usul::Pointers::reference ( _raster );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Tile::Tile ( const Tile &tile, const osg::CopyOp &option ) : BaseClass ( tile, option ),
  _mutex ( new Tile::Mutex ),
  _body ( tile._body ),
  _min ( tile._min ),
  _max ( tile._max ),
  _splitDistance ( tile._splitDistance ),
  _mesh ( new OsgTools::Mesh ( tile._mesh->rows(), tile._mesh->columns() ) ),
  _level ( tile._level ),
  _dirty ( true ),
  _raster ( tile._raster ),
  _children ( tile._children )
{
  USUL_TRACE_SCOPE;

  // Remove if you are ready to test copying. Right now, I'm not sure what 
  // it means. This constructor is here to satisfy the META_Node macro.
  USUL_ASSERT ( false );

  Usul::Pointers::reference ( _raster );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Tile::~Tile()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Tile::_destroy ), "2021499927" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a random material
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  osg::Material* material()
  {
    osg::Vec4 emissive ( 0.0f, 0.0f, 0.0f, 1.0f );
    osg::Vec4 specular ( 0.2f, 0.2f, 0.2f, 1.0f );

    Usul::Adaptors::Random < float > rd ( 0.2f, 1.0f );

	  osg::Vec4 diffuse ( rd(), rd(), rd(), 1.0f );
    osg::Vec4 ambient ( diffuse );

    osg::ref_ptr<osg::Material> mat ( new osg::Material );
	  mat->setAmbient   ( osg::Material::FRONT_AND_BACK, ambient  );
	  mat->setDiffuse   ( osg::Material::FRONT_AND_BACK, diffuse  );
	  mat->setEmission  ( osg::Material::FRONT_AND_BACK, emissive );
	  mat->setSpecular  ( osg::Material::FRONT_AND_BACK, specular );
    mat->setShininess ( osg::Material::FRONT_AND_BACK, 100      );

    return mat.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the tile.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_update()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle not being dirty.
  if ( false == this->dirty() )
    return;

  // Handle bad state.
  if ( ( 0x0 == _body ) || ( 0x0 == _mesh ) )
    return;

  // Remove all the children.
  this->removeChildren ( 0, this->getNumChildren() );

  // Add internal geometry.
  OsgTools::Mesh &mesh ( *_mesh );
  for ( int i = mesh.rows() - 1; i >= 0; --i )
  {
    const double u ( 1.0 - static_cast<double> ( i ) / ( mesh.rows() - 1 ) );
    for ( unsigned int j = 0; j < mesh.columns(); ++j )
    {
      const double v ( static_cast<double> ( j ) / ( mesh.columns() - 1 ) );
      const double lon ( _min[0] + u * ( _max[0] - _min[0] ) );
      const double lat ( _min[1] + v * ( _max[1] - _min[1] ) );

      // Convert lat-lon coordinates to xyz.
      osg::Vec3f &p ( mesh.point ( i, j ) );
      double x ( 0 ), y ( 0 ), z ( 0 );
      const double elevation ( _body->geodeticRadius ( lat ) );
      _body->latLonHeightToXYZ ( lat, lon, elevation, p );

      // Assign normal vectors.
      osg::Vec3f &n ( mesh.normal ( i, j ) );
      n = p; // Minus the center, which is (0,0,0).
      n.normalize();

      // Assign texture coordinate.
      mesh.texCoord ( i, j ).set ( u, 1.0 - v );
    }
  }

  // Depth of skirt.  This function needs to be tweeked.
  double offset ( 25000 - ( this->level() * 150 ) );

  osg::ref_ptr < osg::Group > group ( new osg::Group );
  group->addChild ( this->_buildLonSkirt ( _min[0], 0.0, offset ) ); // Left skirt.
  group->addChild ( this->_buildLonSkirt ( _max[0], 1.0, offset ) ); // Right skirt.
  group->addChild ( this->_buildLatSkirt ( _min[1], 0.0, offset ) ); // Bottom skirt.
  group->addChild ( this->_buildLatSkirt ( _max[1], 1.0, offset ) ); // Top skirt.
  group->addChild ( mesh() );

  // Add mesh geometry to this node.
  this->addChild ( group.get() );

  // Assign material.
  //this->getOrCreateStateSet()->setAttributeAndModes ( Helper::material(), osg::StateAttribute::PROTECTED );

  // Turn off lighting.
  OsgTools::State::StateSet::setLighting ( this, false );

  // Get the image.
  if ( 0x0 != _raster )
  {
    unsigned int width  ( 256 );
    unsigned int height ( 256 );
    Extents extents ( _min, _max );

    osg::ref_ptr < osg::Image > image ( _raster->texture ( extents, width, height, this->level() ) );
    if ( image.valid() )
    {
      // Create the texture.
      osg::ref_ptr < osg::Texture2D > texture ( new osg::Texture2D );
      texture->setImage( image.get() );

      texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
      texture->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
      texture->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
      texture->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );
      texture->setBorderColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 0.0 ) );
      texture->setBorderWidth ( 0.0 );

      // Get the state set.
      osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );
      ss->setTextureAttributeAndModes ( 0, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
    }
  }

  // No longer dirty.
  this->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_destroy()
{
  USUL_TRACE_SCOPE;

  Usul::Pointers::unreference ( _raster ); _raster = 0x0;

  _body = 0x0; // Don't delete!
  delete _mesh; _mesh = 0x0;
  delete _mutex; _mutex = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse the children.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::traverse ( osg::NodeVisitor &nv )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Determine visitor type.
  switch ( nv.getVisitorType() )
  {
    case osg::NodeVisitor::CULL_VISITOR:
    {
      this->_update();
      this->_cull ( nv );
      return;
    }

    default:
    {
      BaseClass::traverse ( nv );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cull traversal.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_cull ( osg::NodeVisitor &nv )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle bad state.
  if ( ( 0x0 == _mesh ) || 
       ( _mesh->rows() < 2 ) || 
       ( _mesh->columns() < 2 ) || 
       ( osg::NodeVisitor::CULL_VISITOR != nv.getVisitorType() ) )
  {
    return;
  }

  // Get cull visitor.
  osgUtil::CullVisitor &cv ( dynamic_cast < osgUtil::CullVisitor & > ( nv ) );

  // Return if we are culled.
  if ( cv.isCulled ( *this ) )
    return;

  // Four corners and center of the tile.
  OsgTools::Mesh &mesh ( *_mesh );
  const osg::Vec3f &eye ( cv.getViewPointLocal() );
  const osg::Vec3f &p00 ( mesh.point ( 0, 0 ) );
  const osg::Vec3f &p0N ( mesh.point ( 0, mesh.columns() - 1 ) );
  const osg::Vec3f &pN0 ( mesh.point ( mesh.rows() - 1, 0 ) );
  const osg::Vec3f &pNN ( mesh.point ( mesh.rows() - 1, mesh.columns() - 1 ) );
  const osg::Vec3f &pBC ( this->getBound().center() );

  // Squared distances from the eye to the points.
  const float dist00 ( ( eye - p00 ).length2() );
  const float dist0N ( ( eye - p0N ).length2() );
  const float distN0 ( ( eye - pN0 ).length2() );
  const float distNN ( ( eye - pNN ).length2() );
  const float distBC ( ( eye - pBC ).length2() );

  // Check with smallest distance.
  const float dist ( Usul::Math::minimum ( dist00, dist0N, distN0, distNN, distBC ) );
  const bool low ( ( dist > ( _splitDistance * _splitDistance ) ) );
  const unsigned int numChildren ( this->getNumChildren() );
  USUL_ASSERT ( numChildren > 0 );

  if ( low )
  {
    // Remove high level of detail.
    this->removeChild ( 1, numChildren - 1 );

    // Unreference all children.
    _children[LOWER_LEFT] = 0x0; _children[LOWER_RIGHT] = 0x0; _children[UPPER_LEFT] = 0x0; _children[UPPER_RIGHT] = 0x0;

    // Traverse low level of detail.
    this->getChild ( 0 )->accept ( nv );
  }

  else
  {
    // Add high level if necessary.
    if ( 1 == numChildren )
    {
      osg::ref_ptr<osg::Group> group ( new osg::Group );
      const double half ( _splitDistance * 0.5f );
      const osg::Vec2d mid ( ( _max + _min ) * 0.5 );
      _children[LOWER_LEFT]  = new Tile ( _level + 1, osg::Vec2d ( _min[0], _min[1] ), osg::Vec2d (  mid[0],  mid[1] ), mesh.rows(), mesh.columns(), half, _body, _raster ); // lower left  tile
      _children[LOWER_RIGHT] = new Tile ( _level + 1, osg::Vec2d (  mid[0], _min[1] ), osg::Vec2d ( _max[0],  mid[1] ), mesh.rows(), mesh.columns(), half, _body, _raster ); // lower right tile
      _children[UPPER_LEFT]  = new Tile ( _level + 1, osg::Vec2d ( _min[0],  mid[1] ), osg::Vec2d (  mid[0], _max[1] ), mesh.rows(), mesh.columns(), half, _body, _raster ); // upper left  tile
      _children[UPPER_RIGHT] = new Tile ( _level + 1, osg::Vec2d (  mid[0],  mid[1] ), osg::Vec2d ( _max[0], _max[1] ), mesh.rows(), mesh.columns(), half, _body, _raster ); // upper right tile
      group->addChild ( _children[LOWER_LEFT]  );
      group->addChild ( _children[LOWER_RIGHT] );
      group->addChild ( _children[UPPER_LEFT]  );
      group->addChild ( _children[UPPER_RIGHT] );
      this->addChild ( group.get() );
    }

    // Traverse last child.
    this->getChild ( this->getNumChildren() - 1 )->accept ( nv );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the level. Zero is the top.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Tile::level() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _level;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the mutex.
//
///////////////////////////////////////////////////////////////////////////////

Tile::Mutex &Tile::mutex() const
{
  USUL_TRACE_SCOPE;
  return *_mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag that says we're dirty.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::dirty ( bool state, bool dirtyChildren )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _dirty = state;

  if ( dirtyChildren )
  {
    if ( _children[LOWER_LEFT].valid()  ) _children[LOWER_LEFT]->dirty  ( state, dirtyChildren );
    if ( _children[LOWER_RIGHT].valid() ) _children[LOWER_RIGHT]->dirty ( state, dirtyChildren );
    if ( _children[UPPER_LEFT].valid()  ) _children[UPPER_LEFT]->dirty  ( state, dirtyChildren );
    if ( _children[UPPER_RIGHT].valid() ) _children[UPPER_RIGHT]->dirty ( state, dirtyChildren );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Mark the dirty state, only if we cross this extents.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::dirty ( bool state, const Extents& extents )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  Extents e ( _min, _max );

  if ( extents.intersects ( e ) )
  {
    // Set our dirty state.
    this->dirty ( state, false );

    // Visit our children.
    if ( _children[LOWER_LEFT].valid()  ) _children[LOWER_LEFT]->dirty  ( state, extents );
    if ( _children[LOWER_RIGHT].valid() ) _children[LOWER_RIGHT]->dirty ( state, extents );
    if ( _children[UPPER_LEFT].valid()  ) _children[UPPER_LEFT]->dirty  ( state, extents );
    if ( _children[UPPER_RIGHT].valid() ) _children[UPPER_RIGHT]->dirty ( state, extents );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flag that says we're dirty.
//
///////////////////////////////////////////////////////////////////////////////

bool Tile::dirty() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build skirt along longitude line.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Tile::_buildLonSkirt ( double lon, double u, double offset )
{
  unsigned int columns ( _mesh->columns() );

  // Make the mesh
  OsgTools::Mesh mesh ( 2, columns );

  // Left skirt.
  for ( unsigned int j = 0; j < columns; ++j )
  {
    const double v ( static_cast<double> ( j ) / ( columns - 1 ) );
    const double lat ( _min[1] + v * ( _max[1] - _min[1] ) );

    const double elevation ( _body->geodeticRadius ( lat ) );

    // Convert lat-lon coordinates to xyz.
    _body->latLonHeightToXYZ ( lat, lon, elevation,          mesh.point ( 0, j ) );
    _body->latLonHeightToXYZ ( lat, lon, elevation - offset, mesh.point ( 1, j ) );

    // Assign texture coordinate.
    mesh.texCoord ( 0, j ).set ( u, v );
    mesh.texCoord ( 1, j ).set ( u, v );
  }

  return mesh();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build skirt along latitude line.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Tile::_buildLatSkirt ( double lat, double v, double offset )
{
  unsigned int rows ( _mesh->rows() );

  // Make the mesh
  OsgTools::Mesh mesh ( rows, 2 );

  // Left skirt.
  for ( int i = rows - 1; i >= 0; --i )
  {
    const double u ( 1.0 - static_cast<double> ( i ) / ( mesh.rows() - 1 ) );
    const double lon ( _min[0] + u * ( _max[0] - _min[0] ) );

    const double elevation ( _body->geodeticRadius ( lat ) );

    // Convert lat-lon coordinates to xyz.
    _body->latLonHeightToXYZ ( lat, lon, elevation,          mesh.point ( i, 0 ) );
    _body->latLonHeightToXYZ ( lat, lon, elevation - offset, mesh.point ( i, 1 ) );

    // Assign texture coordinate.
    mesh.texCoord ( i, 0 ).set ( u, v );
    mesh.texCoord ( i, 1 ).set ( u, v );
  }

  return mesh();
}
