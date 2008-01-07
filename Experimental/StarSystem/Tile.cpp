
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
#include "StarSystem/BuildTiles.h"

#include "OsgTools/Mesh.h"
#include "OsgTools/State/StateSet.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Math/NaN.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Jobs/Manager.h"

#include "ossim/base/ossimEllipsoid.h"

#include "osgUtil/CullVisitor"

#include "osg/Geode"
#include "osg/Material"
#include "osg/Texture2D"

#include <limits>

using namespace StarSystem;

USUL_IMPLEMENT_TYPE_ID ( Tile );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Tile::Tile ( unsigned int level, const Extents &extents, 
             const MeshSize &meshSize, const Usul::Math::Vec4d& texCoords, double splitDistance, 
             Body *body, osg::Image* image, osg::Image * elevation ) : BaseClass(),
  _mutex ( new Tile::Mutex ),
  _body ( body ),
  _extents ( extents ),
  _splitDistance ( splitDistance ),
  _mesh ( new OsgTools::Mesh ( meshSize[0], meshSize[1] ) ),
  _level ( level ),
  _flags ( Tile::ALL ),
  _children(),
  _textureUnit ( 0 ),
  _image ( image ),
  _elevation ( elevation ),
  _texture ( new osg::Texture2D ),
  _texCoords ( texCoords ),
  _jobId ( false, 0 ),
  _elevationJob ( 0x0 ),
  _tileJob ( 0x0 ),
  _boundingSphere()
{
  USUL_TRACE_SCOPE;

  // We want thread safe ref and unref.
  this->setThreadSafeRefUnref ( true );

  // Create the texture.
  _texture->setImage ( _image.get() );
  _texture->setFilter ( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
  _texture->setFilter ( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
  _texture->setWrap ( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
  _texture->setWrap ( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );

  // Turn off back-face culling.
  this->getOrCreateStateSet()->setMode ( GL_CULL_FACE, osg::StateAttribute::OFF );

  // For some reason this is now needed or else we cannot see the images.
  OsgTools::State::StateSet::setMaterialDefault ( this );
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
  _children ( tile._children ),
  _textureUnit ( tile._textureUnit ),
  _image ( tile._image ),
  _elevation ( tile._elevation ),
  _texCoords ( tile._texCoords ),
  _jobId ( false, 0 ),
  _elevationJob ( tile._elevationJob ),
  _tileJob ( tile._tileJob ),
  _boundingSphere ( tile._boundingSphere )
{
  USUL_TRACE_SCOPE;

  // Remove if you are ready to test copying. Right now, I'm not sure what 
  // it means. This constructor is here to satisfy the META_Node macro.
  USUL_ASSERT ( false );
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
//  Update the mesh.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::updateMesh()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle not being dirty.
  if ( false == this->verticesDirty() || false == this->texCoordsDirty() )
    return;

  // Handle bad state.
  if ( ( 0x0 == _body ) || ( 0x0 == _mesh ) )
    return;

  // Remove all the children.
  this->removeChildren ( 0, this->getNumChildren() );

  // Shortcuts.
  const Extents::Vertex &mn ( _extents.minimum() );
  const Extents::Vertex &mx ( _extents.maximum() );

  const double deltaU ( _texCoords[1] - _texCoords[0] );
  const double deltaV ( _texCoords[3] - _texCoords[2] );
  
  // Clear the bounding sphere.
  _boundingSphere.init();

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

      // Only set the vertices if they are dirty.
      if ( this->verticesDirty() )
      {
        // Convert lat-lon coordinates to xyz.
        osg::Vec3f &p ( mesh.point ( i, j ) );
        
        const double elevation ( ( _elevation.valid() ? ( *reinterpret_cast < const float * > ( _elevation->data ( i, j ) ) ) : 0.0 ) );
        _body->latLonHeightToXYZ ( lat, lon, elevation, p );
        
        // Expand the bounding sphere by the point.
        _boundingSphere.expandBy ( p );

        // Assign normal vectors.
        osg::Vec3f &n ( mesh.normal ( i, j ) );
        n = p; // Minus the center, which is (0,0,0).
        n.normalize();
      }

      // Only set the texture coordinates if they are dirty.
      if ( this->texCoordsDirty() )
      {
        // Assign texture coordinate.  Lower left corner should be (0,0).
        const double s ( ( _texCoords[0] + ( u * deltaU ) ) );
        const double t ( ( _texCoords[2] + ( v * deltaV ) ) );
        mesh.texCoord ( i, j ).set ( s , t );
      }
    }
  }

  // Unset these dirty flags.
  this->dirty ( false, Tile::VERTICES, false );
  this->dirty ( false, Tile::TEX_COORDS, false );

  // Make group to hold the meshes.
  osg::ref_ptr < osg::Group > group ( new osg::Group );

  // Build skirts if we're supposed to.
  if ( true == _body->useSkirts() )
  {
    // Depth of skirt.  TODO: This function needs to be tweeked.
    const double offset ( Usul::Math::maximum<double> ( ( 5000 - ( this->level() * 150 ) ), ( 10 * std::numeric_limits<double>::epsilon() ) ) );

    // Add skirts to group.
    //group->addChild ( this->_buildLonSkirt ( _extents.minimum()[0], _texCoords[0], mesh.rows() - 1,    offset ) ); // Left skirt.
    group->addChild ( this->_buildLonSkirt ( _extents.maximum()[0], _texCoords[1], 0,                  offset ) ); // Right skirt.
    group->addChild ( this->_buildLatSkirt ( _extents.minimum()[1], _texCoords[2], 0,                  offset ) ); // Bottom skirt.
    //group->addChild ( this->_buildLatSkirt ( _extents.maximum()[1], _texCoords[3], mesh.columns() - 1, offset ) ); // Top skirt.
  }
  
  // Make the ground.
  group->addChild ( mesh() );

  // Add the group to us.
  this->addChild ( group.get() );
  
  this->dirtyBound();
}


//////////////////////////////////////////////////////////////////////////////
//
//  Update the texture.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::updateTexture()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  // Get the image.
  if ( ( true == _texture.valid() ) && ( 0x0 != _texture->getImage() ) && ( true == this->textureDirty() ) )
  {
    // Get the state set.
    osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );
    ss->setTextureAttributeAndModes ( _textureUnit, _texture.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

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
  this->clear();

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

  // Launch the image request if one is needed.
  if ( ( true == this->textureDirty() ) && ( false == _jobId.first ) )
  {
    this->_launchImageRequest();
  }

  // If it's a cull visitor...
  if ( osg::NodeVisitor::CULL_VISITOR == nv.getVisitorType() )
  {
    // See if our job is done loading image.
    if ( ( 0x0 != _body ) && ( true == _jobId.first ) )
    {
      osg::ref_ptr < osg::Texture2D > texture ( _body->texture ( _jobId.second ) );
      if ( texture.valid() )
      {
        this->textureData ( texture.get(), Usul::Math::Vec4d ( 0.0, 1.0, 0.0, 1.0 ) );
        _jobId.first = false;
      }
    }
    
    // Check for new elevation data.
    if ( _elevationJob.valid() && _elevationJob->isDone() )
    {
      _elevation = _elevationJob->image();
      _elevationJob = 0x0;
      
      // Force vertices to be rebuilt.
      _flags = Usul::Bits::set ( _flags, Tile::VERTICES, true );
    }
    
    // Get cull visitor.
    osgUtil::CullVisitor *cv ( dynamic_cast < osgUtil::CullVisitor * > ( &nv ) );
    
    // Return if we are culled.
    //if ( 0x0 == cv || cv->isCulled ( *this ) )
      //return;
    
    // Only update here if the vertices are invalid.
    // This will ensure proper splitting.
    this->updateMesh();
    
    //cv->setComputeNearFarMode ( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
    
    this->_cull ( *cv );
    
    //cv->setComputeNearFarMode ( osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES );
  }
  else
  {
    BaseClass::traverse ( nv );
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
       ( _mesh->columns() < 2 ) ||
       ( 0x0 == _body ) ||
       ( 0x0 == _body->jobManager() ) )
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
  const bool farAway ( ( dist > ( _splitDistance * _splitDistance ) ) );
  const unsigned int numChildren ( this->getNumChildren() );
  USUL_ASSERT ( numChildren > 0 );

  // Check for nan in eye values (it's happened).
  const bool eyeIsNan ( Usul::Math::nan ( eye[0] ) || Usul::Math::nan ( eye[1] ) || Usul::Math::nan ( eye[2] ) );

  // Check if we've gone too deep.
  const bool tooDeep ( this->level() >= _body->maxLevel() );

  // Should we traverse the low lod?
  bool low ( farAway || eyeIsNan || tooDeep );

  // Finally, ask the callback.
  low = !( _body->shouldSplit ( !low, this ) );

  if ( low )
  {    
    this->updateTexture();

    // Remove high level of detail.
    if ( numChildren > 1 )
    {
      if ( false == _body->cacheTiles() )
      {
        this->removeChild ( 1, numChildren - 1 );

        // Clear all the children.
        if ( _children[LOWER_LEFT].valid()  ) _children[LOWER_LEFT]->clear();
        if ( _children[LOWER_RIGHT].valid() ) _children[LOWER_RIGHT]->clear();
        if ( _children[UPPER_LEFT].valid()  ) _children[UPPER_LEFT]->clear();
        if ( _children[UPPER_RIGHT].valid() ) _children[UPPER_RIGHT]->clear();

        _children[LOWER_LEFT]  = 0x0;
        _children[LOWER_RIGHT] = 0x0;
        _children[UPPER_LEFT]  = 0x0;
        _children[UPPER_RIGHT] = 0x0;

        // Clear the tile job.
        if ( _tileJob.valid() ) _tileJob->cancel(); _tileJob = 0x0;
      }
    }
  }

  else
  {
    // Add high level if necessary.
    if ( 1 == numChildren )
    {
      // Make tiles if we are not caching them, or if this is the first time.
      if ( ( false == _body->cacheTiles() ) || 
           ( false == _children[LOWER_LEFT].valid()  ) ||
           ( false == _children[LOWER_RIGHT].valid() ) ||
           ( false == _children[UPPER_LEFT].valid()  ) ||
           ( false == _children[UPPER_RIGHT].valid() ) )
      {
        // Use the low lod while we are waiting for the job.
        if ( _tileJob.valid() )
        {
          low = true;
        }
        else
        {
          _tileJob = new StarSystem::BuildTiles ( Tile::RefPtr ( this ) );
        
          // Add the job to the job manager.
          _body->jobManager()->addJob ( _tileJob.get() );
        }
      }

      if ( _tileJob.valid() && _tileJob->isDone() )
      {
        osg::ref_ptr<osg::Group> group ( new osg::Group );
        group->addChild ( _children[LOWER_LEFT]  );
        group->addChild ( _children[LOWER_RIGHT] );
        group->addChild ( _children[UPPER_LEFT]  );
        group->addChild ( _children[UPPER_RIGHT] );
        this->addChild ( group.get() );
        _tileJob = 0x0;
      }
    }
  }

  // Traverse low level of detail.
  if ( low || _tileJob.valid() )
  {
#if 0
    osg::BoundingBox bb;
    osg::BoundingSphere bs ( this->getBound() );
    bb.expandBy ( osg::BoundingSphere ( bs.center(), bs.radius() * 0.5 ) );
    cv.updateCalculatedNearFar ( *cv.getModelViewMatrix(), bb );
#endif
    
#if 0
    cv.updateCalculatedNearFar ( p00 );
    cv.updateCalculatedNearFar ( p0N );
    cv.updateCalculatedNearFar ( pN0 );
    cv.updateCalculatedNearFar ( pNN );
#endif
    
    this->getChild ( 0 )->accept ( cv );
  }

  // Traverse last child.
  else
  {
    const unsigned int last ( this->getNumChildren() - 1 );
    this->getChild ( last )->accept ( cv );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Split the tile.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::split ( Usul::Jobs::Job::RefPtr job )
{
  Body::RefPtr body ( Usul::Threads::Safe::get ( this->mutex(), _body ) );
  
  // Handle no body.
  if ( false == body.valid() )
    return;
  
  const double half ( this->splitDistance() * 0.5 );
  
  const Extents extents ( this->extents() );
  
  const Extents::Vertex &mn ( extents.minimum() );
  const Extents::Vertex &mx ( extents.maximum() );
  const Extents::Vertex md ( ( mx + mn ) * 0.5 );
  
  const unsigned int level ( this->level() + 1 );
  
  // Extents for children tiles.
  const Extents ll ( Extents::Vertex ( mn[0], mn[1] ), Extents::Vertex ( md[0], md[1] ) );
  const Extents lr ( Extents::Vertex ( md[0], mn[1] ), Extents::Vertex ( mx[0], md[1] ) );
  const Extents ul ( Extents::Vertex ( mn[0], md[1] ), Extents::Vertex ( md[0], mx[1] ) );
  const Extents ur ( Extents::Vertex ( md[0], md[1] ), Extents::Vertex ( mx[0], mx[1] ) );
  
  // Mesh sizes for children tiles.
  const MeshSize mll ( body->meshSize ( ll ) );
  const MeshSize mlr ( body->meshSize ( lr ) );
  const MeshSize mul ( body->meshSize ( ul ) );
  const MeshSize mur ( body->meshSize ( ur ) );
  
  // Texture coordinates for the children tiles.
  Usul::Math::Vec4d tll, tlr, tul, tur;
  this->_quarterTextureCoordinates ( tll, tlr, tul, tur );

  Tile::RefPtr t0 ( this->_buildTile ( level, ll, mll, tll, half, job ) ); // lower left  tile
  Tile::RefPtr t1 ( this->_buildTile ( level, lr, mlr, tlr, half, job ) ); // lower right tile
  Tile::RefPtr t2 ( this->_buildTile ( level, ul, mul, tul, half, job ) ); // upper left  tile
  Tile::RefPtr t3 ( this->_buildTile ( level, ur, mur, tur, half, job ) ); // upper right tile
  
  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();
  
  {
    Guard guard ( this->mutex() );
    _children[LOWER_LEFT]  = t0.get(); 
    _children[LOWER_RIGHT] = t1.get();
    _children[UPPER_LEFT]  = t2.get();  
    _children[UPPER_RIGHT] = t3.get();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a tile.
//
///////////////////////////////////////////////////////////////////////////////

Tile::RefPtr Tile::_buildTile ( unsigned int level, const Extents& extents, const MeshSize& size, const Usul::Math::Vec4d& texCoords, double splitDistance, Usul::Jobs::Job::RefPtr job ) const
{
  Body::RefPtr body ( Usul::Threads::Safe::get ( this->mutex(), _body ) );
  
  // Handle no body.
  if ( false == body.valid() )
    return 0x0;
  
  // The texture coordinates to use.
  Usul::Math::Vec4d tCoords ( 0.0, 1.0, 0.0, 1.0 );
  
  RasterLayer::RefPtr raster ( body->rasterData() );
  osg::ref_ptr < osg::Image > image ( 0x0 );
  
  if ( raster.valid() )
  {
    const unsigned int width ( 512 );
    const unsigned int height ( 512 );
    image = raster->texture ( extents, width, height, level, job );
  }
  
  // If we didn't get an image, use our image.
  if ( false == image.valid() )
  {
    image = Usul::Threads::Safe::get ( this->mutex(), _image.get() );
    
    // Use the suggested texture coordinates.
    tCoords.set ( texCoords[0], texCoords[1], texCoords[2], texCoords[3] );
  }
  
  osg::ref_ptr < osg::Image > elevation ( 0x0 );
  RasterLayer::RefPtr elevationData ( body->elevationData() );
  
  if ( elevationData.valid() )
  {
    MeshSize size ( this->meshSize() );
    elevation = elevationData->texture ( extents, size[0], size[1], level, job );
  }
  
  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();
  
  // Make the tile.
  Tile::RefPtr tile ( new Tile ( level, extents, size, tCoords, splitDistance, body.get(), image.get(), elevation.get() ) );
  tile->updateMesh();
  tile->updateTexture();
  
  return tile;
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

osg::Node* Tile::_buildLonSkirt ( double lon, double u, unsigned int i, double offset )
{
  const unsigned int columns ( _mesh->columns() );

  // Make the mesh
  OsgTools::Mesh mesh ( 2, columns );

  // Left skirt.
  for ( unsigned int j = 0; j < columns; ++j )
  {
    const double v ( static_cast<double> ( j ) / ( columns - 1 ) );
    const double lat ( _extents.minimum()[1] + v * ( _extents.maximum()[1] - _extents.minimum()[1] ) );

    const double elevation ( ( _elevation.valid() ? ( *reinterpret_cast < const float * > ( _elevation->data ( i, j ) ) ) : 0.0 ) );

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

osg::Node* Tile::_buildLatSkirt ( double lat, double v, unsigned int j, double offset )
{
  const unsigned int rows ( _mesh->rows() );

  // Make the mesh
  OsgTools::Mesh mesh ( rows, 2 );

  // Left skirt.
  for ( int i = rows - 1; i >= 0; --i )
  {
    const double u ( 1.0 - static_cast<double> ( i ) / ( mesh.rows() - 1 ) );
    const double lon ( _extents.minimum()[0] + u * ( _extents.maximum()[0] - _extents.minimum()[0] ) );

    const double elevation ( ( _elevation.valid() ? ( *reinterpret_cast < const float * > ( _elevation->data ( i, j ) ) ) : 0.0 ) );

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

  // Get copy of job id and set the member to default.
  JobID job ( false, 0 );
  {
    Guard guard ( this );
    job = _jobId;
    _jobId.first = false;
  }

  // Cancel job if it's valid.
  if ( true == job.first )
  {
    _body->textureRequestCancel ( job.second );
  }

  if ( _tileJob.valid() )
  {
    _tileJob->cancel();
    _tileJob = 0x0;
  }

  this->dirty ( false, Tile::ALL, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the texture.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::texture ( osg::Texture2D* texture )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 != texture )
  {
    _texture = texture;
    this->dirty ( true, Tile::TEXTURE, false );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

osg::Texture2D* Tile::texture()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _texture.get();
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

  // Start the request to pull in texture.
  if ( 0x0 != _body )
  {
    if ( true == _jobId.first )
    {
      _body->textureRequestCancel ( _jobId.second );
      _jobId.first = false;
    }

    _jobId.second = _body->textureRequest ( this->extents(), this->level() );
    _jobId.first = true;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the elevation.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_launchElevationRequest()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Start the request to pull in texture.
  if ( ( 0x0 != _body ) && ( 0x0 != _body->jobManager() ) )
  {
    MeshSize size ( this->meshSize() );
    _elevationJob = new CutImageJob ( this->extents(), size[0], size[1], this->level(), _body->elevationData() );
    _body->jobManager()->addJob ( _elevationJob.get() );
  }
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* Tile::image()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _image.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Quarter the texture coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_quarterTextureCoordinates ( Usul::Math::Vec4d& ll, Usul::Math::Vec4d& lr, Usul::Math::Vec4d& ul, Usul::Math::Vec4d& ur ) const
{
  USUL_TRACE_SCOPE;

  const double deltaU ( _texCoords[1] - _texCoords[0] );
  const double deltaV ( _texCoords[3] - _texCoords[2] );

  const double startU ( _texCoords[0] );
  const double halfU  ( _texCoords[0] + ( deltaU / 2.0 ) );
  const double endU   ( startU + deltaU );

  const double startV ( _texCoords[2] );
  const double halfV  ( _texCoords[2] + ( deltaV / 2.0 ) );
  const double endV   ( startV + deltaV );

  ll.set ( startU, halfU, startV, halfV );
  lr.set ( halfU,  endU,  startV, halfV );
  ul.set ( startU, halfU, halfV,  endV );
  ur.set ( halfU,  endU,  halfV,  endV );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the texture data.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::textureData ( osg::Texture2D* texture, const Usul::Math::Vec4d& coords )
{
  USUL_TRACE_SCOPE;

  // Set the texture.
  this->texture ( texture );

  // Set the texture coordinates.
  this->texCoords ( coords );
  
#if 1
  // Set children data.
  {
    Guard guard ( this );

    Usul::Math::Vec4d ll, lr, ul, ur;
    this->_quarterTextureCoordinates ( ll, lr, ul, ur );
    if ( _children[LOWER_LEFT].valid()  && this->image() == _children[LOWER_LEFT]->image()  ) _children[LOWER_LEFT]->textureData  ( texture, ll );
    if ( _children[LOWER_RIGHT].valid() && this->image() == _children[LOWER_RIGHT]->image() ) _children[LOWER_RIGHT]->textureData ( texture, lr );
    if ( _children[UPPER_LEFT].valid()  && this->image() == _children[UPPER_LEFT]->image()  ) _children[UPPER_LEFT]->textureData  ( texture, ul );
    if ( _children[UPPER_RIGHT].valid() && this->image() == _children[UPPER_RIGHT]->image() ) _children[UPPER_RIGHT]->textureData ( texture, ur );
  }
#endif

  // Set our image.
  this->image ( texture->getImage() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

Tile::MeshSize Tile::meshSize() const
{
  USUL_TRACE_SCOPE;

  MeshSize size ( 0, 0 );

  Guard guard ( this );

  // Set the size.
  if ( 0x0 != _mesh )
    size.set ( _mesh->rows(), _mesh->columns() );

  return size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the split distance.
//
///////////////////////////////////////////////////////////////////////////////

double Tile::splitDistance() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _splitDistance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

osg::BoundingSphere Tile::computeBound() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _boundingSphere;
}
