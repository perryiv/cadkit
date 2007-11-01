
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
#include "Usul/Bits/Bits.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Jobs/Manager.h"

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

Tile::Tile ( unsigned int level, const Extents &extents, 
             const MeshSize &meshSize, const Usul::Math::Vec4d& texCoords, double splitDistance, 
             Body *body, RasterLayer* raster, osg::Image* image ) : BaseClass(),
  _mutex ( new Tile::Mutex ),
  _body ( body ),
  _extents ( extents ),
  _splitDistance ( splitDistance ),
  _mesh ( new OsgTools::Mesh ( meshSize[0], meshSize[1] ) ),
  _level ( level ),
  _flags ( Tile::ALL ),
  _raster ( raster ),
  _children (),
  _textureUnit ( 0 ),
  _image ( image ),
  _texCoords ( texCoords ),
  _cutImageJob ( 0x0 )
{
  USUL_TRACE_SCOPE;

  Usul::Pointers::reference ( _raster );

  // We want thread safe ref and unref.
  this->setThreadSafeRefUnref ( true );

  // Start the request to pull in texture.
  this->_launchImageRequest();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Tile::Tile ( const Tile &tile, const osg::CopyOp &option ) : BaseClass ( tile, option ),
  _mutex ( new Tile::Mutex ),
  _body ( tile._body ),
  _extents ( tile._extents ),
  _splitDistance ( tile._splitDistance ),
  _mesh ( new OsgTools::Mesh ( tile._mesh->rows(), tile._mesh->columns() ) ),
  _level ( tile._level ),
  _flags ( Tile::ALL ),
  _raster ( tile._raster ),
  _children ( tile._children ),
  _textureUnit ( tile._textureUnit ),
  _image ( tile._image ),
  _texCoords ( tile._texCoords ),
  _cutImageJob ( tile._cutImageJob )
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

  // Shortcuts.
  const Extents::Vertex &mn ( _extents.minimum() );
  const Extents::Vertex &mx ( _extents.maximum() );

  double deltaU ( _texCoords[1] - _texCoords[0] );
  double deltaV ( _texCoords[3] - _texCoords[2] );

  // Add internal geometry.
  OsgTools::Mesh &mesh ( *_mesh );
  for ( int i = mesh.rows() - 1; i >= 0; --i )
  {
    const double u ( 1.0 - static_cast<double> ( i ) / ( mesh.rows() - 1 ) );
    for ( unsigned int j = 0; j < mesh.columns(); ++j )
    {
      const double v ( static_cast<double> ( j ) / ( mesh.columns() - 1 ) );
      const double lon ( mn[0] + u * ( mx[0] - mn[0] ) );
      const double lat ( mn[1] + v * ( mx[1] - mn[1] ) );

      // Only set the vertices, if they are dirty.
      if ( this->verticesDirty() )
      {
        // Convert lat-lon coordinates to xyz.
        osg::Vec3f &p ( mesh.point ( i, j ) );
        double x ( 0 ), y ( 0 ), z ( 0 );
        const double elevation ( _body->geodeticRadius ( lat ) );
        _body->latLonHeightToXYZ ( lat, lon, elevation, p );

        // Assign normal vectors.
        osg::Vec3f &n ( mesh.normal ( i, j ) );
        n = p; // Minus the center, which is (0,0,0).
        n.normalize();
      }

      // Only set the texture coordinates if they are dirty.
      if ( this->texCoordsDirty() )
      {
        // Assign texture coordinate.
        double up ( ( _texCoords[0] + ( u * deltaU ) ) );
        double vp ( ( _texCoords[2] + ( ( 1.0 - v ) * deltaV ) ) );
        mesh.texCoord ( i, j ).set ( up , vp );
      }
    }
  }

  // Unset these dirty flags.
  this->dirty ( false, Tile::VERTICES, false );
  this->dirty ( false, Tile::TEX_COORDS, false );

  // Depth of skirt.  This function needs to be tweeked.
  double offset ( 25000 - ( this->level() * 150 ) );

  osg::ref_ptr < osg::Group > group ( new osg::Group );
  group->addChild ( this->_buildLonSkirt ( _extents.minimum()[0], _texCoords[0], offset ) ); // Left skirt.
  group->addChild ( this->_buildLonSkirt ( _extents.maximum()[0], _texCoords[1], offset ) ); // Right skirt.
  group->addChild ( this->_buildLatSkirt ( _extents.minimum()[1], _texCoords[2], offset ) ); // Bottom skirt.
  group->addChild ( this->_buildLatSkirt ( _extents.maximum()[1], _texCoords[3], offset ) ); // Top skirt.
  group->addChild ( mesh() );

  // Add mesh geometry to this node.
  this->addChild ( group.get() );

  // Assign material.
  //this->getOrCreateStateSet()->setAttributeAndModes ( Helper::material(), osg::StateAttribute::PROTECTED );

  // Get the image.
  if ( _image.valid() && this->textureDirty() )
  {
    // Create the texture.
    osg::ref_ptr < osg::Texture2D > texture ( new osg::Texture2D );
    texture->setImage( _image.get() );

    texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
    texture->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
    texture->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
    texture->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );
    texture->setBorderColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 0.0 ) );
    texture->setBorderWidth ( 0.0 );

    // Get the state set.
    osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );
    ss->setTextureAttributeAndModes ( _textureUnit, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

    // Turn off lighting.
    OsgTools::State::StateSet::setLighting ( this, false );

    // Texture no longer dirty.
    this->dirty ( false, Tile::TEXTURE, false );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_destroy()
{
  USUL_TRACE_SCOPE;

  // Cancel the job.
  if ( _cutImageJob.valid() && false == _cutImageJob->isDone() )
    _cutImageJob->cancel();
  _cutImageJob = 0x0;

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
    case osg::NodeVisitor::UPDATE_VISITOR:
    {
      if ( 0 == this->level() && 0x0 != _body )
        _body->jobManager().purge();
      return;
    }

    case osg::NodeVisitor::CULL_VISITOR:
    {
      // See if our job is done loading image.
      if ( _cutImageJob.valid() && _cutImageJob->isDone() )
      {
        this->image ( _cutImageJob->image() );
        this->texCoords ( Usul::Math::Vec4d ( 0.0, 1.0, 0.0, 1.0 ) );
        _cutImageJob = 0x0;
      }

      if ( 0 == this->level() && 0x0 != _body )
        _body->jobManager().purge();

      // Get cull visitor.
      osgUtil::CullVisitor *cv ( dynamic_cast < osgUtil::CullVisitor * > ( &nv ) );

      // Return if we are culled.
      if ( 0x0 == cv || cv->isCulled ( *this ) )
        return;

      // Only update here if the vertices are invalid.
      // This will ensure proper splitting.
      if ( this->verticesDirty() ) 
        this->_update();

      this->_cull ( *cv );
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

void Tile::_cull ( osgUtil::CullVisitor &cv )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle bad state.
  if ( ( 0x0 == _mesh ) || 
       ( _mesh->rows() < 2 ) || 
       ( _mesh->columns() < 2 ) )
  {
    return;
  }

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

  // Make sure texture state is on.
  //osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );
  //ss->setTextureMode ( _textureUnit, GL_TEXTURE_2D, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

  if ( low )
  {
    this->_update();

    // Remove high level of detail.
    if ( numChildren > 1 )
    {
      this->removeChild ( 1, numChildren - 1 );

      // Clear the job and all children.
      if ( _children[LOWER_LEFT].valid()  ) _children[LOWER_LEFT]->clear();
      if ( _children[LOWER_RIGHT].valid() ) _children[LOWER_RIGHT]->clear();
      if ( _children[UPPER_LEFT].valid()  ) _children[UPPER_LEFT]->clear();
      if ( _children[UPPER_RIGHT].valid() ) _children[UPPER_RIGHT]->clear();

      _children[LOWER_LEFT]  = 0x0;
      _children[LOWER_RIGHT] = 0x0;
      _children[UPPER_LEFT]  = 0x0;
      _children[UPPER_RIGHT] = 0x0;
    }

    // Traverse low level of detail.
    this->getChild ( 0 )->accept ( cv );
  }

  else
  {
    //this->clear();

    // Add high level if necessary.
    if ( 1 == numChildren )
    {
      const double half ( _splitDistance * 0.5 );

      const Extents::Vertex &mn ( _extents.minimum() );
      const Extents::Vertex &mx ( _extents.maximum() );
      const Extents::Vertex md ( ( mx + mn ) * 0.5 );

      const unsigned int level ( _level + 1 );
      const MeshSize meshSize ( mesh.rows(), mesh.columns() );

      const Extents ll ( Extents::Vertex ( mn[0], mn[1] ), Extents::Vertex ( md[0], md[1] ) );
      const Extents lr ( Extents::Vertex ( md[0], mn[1] ), Extents::Vertex ( mx[0], md[1] ) );
      const Extents ul ( Extents::Vertex ( mn[0], md[1] ), Extents::Vertex ( md[0], mx[1] ) );
      const Extents ur ( Extents::Vertex ( md[0], md[1] ), Extents::Vertex ( mx[0], mx[1] ) );

      double deltaU ( _texCoords[1] - _texCoords[0] );
      double deltaV ( _texCoords[3] - _texCoords[2] );

      double startU ( _texCoords[0] );
      double halfU  ( _texCoords[0] + ( deltaU / 2.0 ) );
      double endU   ( startU + deltaU );

      double startV ( _texCoords[2] );
      double halfV  ( _texCoords[2] + ( deltaV / 2.0 ) );
      double endV   ( startV + deltaV );

      const Usul::Math::Vec4d tll ( startU, halfU, halfV,  endV );
      const Usul::Math::Vec4d tlr ( halfU,  endU,  halfV,  endV );
      const Usul::Math::Vec4d tul ( startU, halfU, startV, halfV );
      const Usul::Math::Vec4d tur ( halfU,  endU,  startV, halfV );

      _children[LOWER_LEFT]  = new Tile ( level, ll, meshSize, tll, half, _body, _raster, _image.get() ); // lower left  tile
      _children[LOWER_RIGHT] = new Tile ( level, lr, meshSize, tlr, half, _body, _raster, _image.get() ); // lower right tile
      _children[UPPER_LEFT]  = new Tile ( level, ul, meshSize, tul, half, _body, _raster, _image.get() ); // upper left  tile
      _children[UPPER_RIGHT] = new Tile ( level, ur, meshSize, tur, half, _body, _raster, _image.get() ); // upper right tile

      osg::ref_ptr<osg::Group> group ( new osg::Group );
      group->addChild ( _children[LOWER_LEFT]  );
      group->addChild ( _children[LOWER_RIGHT] );
      group->addChild ( _children[UPPER_LEFT]  );
      group->addChild ( _children[UPPER_RIGHT] );
      this->addChild ( group.get() );

      // Remove texture state
      //osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );
      //ss->setTextureMode ( _textureUnit, GL_TEXTURE_2D, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
      //ss->removeTextureMode ( _textureUnit, GL_TEXTURE_2D );
    }

    // Traverse last child.
    this->getChild ( this->getNumChildren() - 1 )->accept ( cv );
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

void Tile::dirty ( bool state, unsigned int flags, bool dirtyChildren )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _flags = Usul::Bits::set ( _flags, flags, state );

  if ( dirtyChildren )
  {
    if ( _children[LOWER_LEFT].valid()  ) _children[LOWER_LEFT]->dirty  ( state, flags, dirtyChildren );
    if ( _children[LOWER_RIGHT].valid() ) _children[LOWER_RIGHT]->dirty ( state, flags, dirtyChildren );
    if ( _children[UPPER_LEFT].valid()  ) _children[UPPER_LEFT]->dirty  ( state, flags, dirtyChildren );
    if ( _children[UPPER_RIGHT].valid() ) _children[UPPER_RIGHT]->dirty ( state, flags, dirtyChildren );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Mark the dirty state, only if we cross this extents.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::dirty ( bool state, unsigned int flags, bool dirtyChildren, const Extents& extents )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( extents.intersects ( _extents ) )
  {
    // Set our dirty state.
    this->dirty ( state, flags, false );

    // Visit our children.
    if ( dirtyChildren )
    {
      if ( _children[LOWER_LEFT].valid()  ) _children[LOWER_LEFT]->dirty  ( state, flags, dirtyChildren, extents );
      if ( _children[LOWER_RIGHT].valid() ) _children[LOWER_RIGHT]->dirty ( state, flags, dirtyChildren, extents );
      if ( _children[UPPER_LEFT].valid()  ) _children[UPPER_LEFT]->dirty  ( state, flags, dirtyChildren, extents );
      if ( _children[UPPER_RIGHT].valid() ) _children[UPPER_RIGHT]->dirty ( state, flags, dirtyChildren, extents );
    }

#if 1
    if ( this->textureDirty() )
    {
      // Start the request to pull in texture.
      this->_launchImageRequest();
    }
#endif
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
  return 0 != _flags;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are the vertices dirty?
//
///////////////////////////////////////////////////////////////////////////////

bool Tile::verticesDirty() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return Usul::Bits::has ( _flags, Tile::VERTICES );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are the texture coordinates dirty?
//
///////////////////////////////////////////////////////////////////////////////

bool Tile::texCoordsDirty() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return Usul::Bits::has ( _flags, Tile::TEX_COORDS );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the texture dirty?
//
///////////////////////////////////////////////////////////////////////////////

bool Tile::textureDirty() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return Usul::Bits::has ( _flags, Tile::TEXTURE );
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
    const double lat ( _extents.minimum()[1] + v * ( _extents.maximum()[1] - _extents.minimum()[1] ) );

    const double elevation ( _body->geodeticRadius ( lat ) );

    // Convert lat-lon coordinates to xyz.
    _body->latLonHeightToXYZ ( lat, lon, elevation,          mesh.point ( 0, j ) );
    _body->latLonHeightToXYZ ( lat, lon, elevation - offset, mesh.point ( 1, j ) );

    // Assign texture coordinate.
    mesh.texCoord ( 0, j ).set ( _texCoords[0] + u, _texCoords[2] + v );
    mesh.texCoord ( 1, j ).set ( _texCoords[0] + u, _texCoords[2] + v );
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
    const double lon ( _extents.minimum()[0] + u * ( _extents.maximum()[0] - _extents.minimum()[0] ) );

    const double elevation ( _body->geodeticRadius ( lat ) );

    // Convert lat-lon coordinates to xyz.
    _body->latLonHeightToXYZ ( lat, lon, elevation,          mesh.point ( i, 0 ) );
    _body->latLonHeightToXYZ ( lat, lon, elevation - offset, mesh.point ( i, 1 ) );

    // Assign texture coordinate.
    mesh.texCoord ( i, 0 ).set ( _texCoords[0] + u, _texCoords[2] + v );
    mesh.texCoord ( i, 1 ).set ( _texCoords[0] + u, _texCoords[2] + v );
  }

  return mesh();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::clear()
{
  USUL_TRACE_SCOPE;

  if ( _cutImageJob.valid() )
    _cutImageJob->cancel();

  _cutImageJob = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the image.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::image ( osg::Image* image )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _image = image;

  this->dirty ( true, Tile::TEXTURE, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* Tile::image ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _image.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

Tile::Extents Tile::extents() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _extents;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the starting texture coordinates.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec4d Tile::texCoords() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _texCoords;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the starting texture coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::texCoords ( const Usul::Math::Vec4d& t )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _texCoords = t;

  this->dirty ( true, Tile::TEX_COORDS, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the image.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_launchImageRequest()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( _cutImageJob.valid() )
    _cutImageJob->cancel();

  _cutImageJob = 0x0;

  // Start the request to pull in texture.
  if ( 0x0 != _body )
  {
    _cutImageJob = new CutImageJob ( this->extents(), this->level(), _raster );
    _body->jobManager().add ( _cutImageJob );
  }
}
