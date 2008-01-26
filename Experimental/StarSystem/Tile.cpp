
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

#include "OsgTools/Group.h"
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

#include "osgUtil/CullVisitor"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/PolygonOffset"
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
  _mesh ( new Mesh ( meshSize[0], meshSize[1] ) ),
  _level ( level ),
  _flags ( Tile::ALL ),
  _children(),
  _textureUnit ( 0 ),
  _image ( image ),
  _elevation ( elevation ),
  _texture ( new osg::Texture2D ),
  _texCoords ( texCoords ),
  _imageJob ( 0x0 ),
  _elevationJob ( 0x0 ),
  _tileJob ( 0x0 ),
  _boundingSphere(),
  _borders ( new osg::Group )
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
  _mesh ( new Mesh ( tile._mesh->rows(), tile._mesh->columns() ) ),
  _level ( tile._level ),
  _flags ( Tile::ALL ),
  _children ( tile._children ),
  _textureUnit ( tile._textureUnit ),
  _image ( tile._image ),
  _elevation ( tile._elevation ),
  _texCoords ( tile._texCoords ),
  _imageJob ( 0x0 ),
  _elevationJob ( 0x0 ),
  _tileJob ( 0x0 ),
  _boundingSphere ( tile._boundingSphere ),
  _borders ( new osg::Group )
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
  _borders = 0x0;
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
  Mesh &mesh ( *_mesh );

  // Clear the bounding sphere.
  _boundingSphere.init();

  // Add internal geometry.
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
        Mesh::Vector &p ( mesh.point ( i, j ) );
        
        const double elevation ( ( _elevation.valid() ? ( *reinterpret_cast < const float * > ( _elevation->data ( i, j ) ) ) : 0.0 ) );
        _body->latLonHeightToXYZ ( lat, lon, elevation, p );
        
        // Expand the bounding sphere by the point.
        _boundingSphere.expandBy ( p );

        // Assign normal vectors.
        Mesh::Vector &n ( mesh.normal ( i, j ) );
        n = p; // Minus the center, which is (0,0,0).
        n.normalize();
      }

      // Only set the texture coordinates if they are dirty.
      if ( this->texCoordsDirty() )
      {
        // Assign texture coordinate.  Lower left corner should be (0,0).
        const double s ( ( _texCoords[0] + ( u * deltaU ) ) );
        const double t ( ( _texCoords[2] + ( v * deltaV ) ) );
        mesh.texCoord ( i, j ).set ( s, t );
      }
    }
  }
  
  Mesh::Vector ll ( mesh.point ( 0, 0 ) );
  for ( int i = mesh.rows() - 1; i >= 0; --i )
  {
    for ( unsigned int j = 0; j < mesh.columns(); ++j )
    {
      Mesh::Vector &p ( mesh.point ( i, j ) );
      p = p - ll;
    }
  }

  // Unset these dirty flags.
  this->dirty ( false, Tile::VERTICES, false );
  this->dirty ( false, Tile::TEX_COORDS, false );

  // Make group to hold the meshes.
  osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );
  mt->setMatrix ( osg::Matrix::translate ( ll ) );

  // Build skirts if we're supposed to.
  if ( true == _body->useSkirts() )
  {
    // Depth of skirt.  TODO: This function needs to be tweeked.
    const double offset ( Usul::Math::maximum<double> ( ( 5000 - ( this->level() * 150 ) ), ( 10 * std::numeric_limits<double>::epsilon() ) ) );

    // Add skirts to group.
    mt->addChild ( this->_buildLonSkirt ( _extents.minimum()[0], _texCoords[0], mesh.rows() - 1,    offset, ll ) ); // Left skirt.
    mt->addChild ( this->_buildLonSkirt ( _extents.maximum()[0], _texCoords[1], 0,                  offset, ll ) ); // Right skirt.
    mt->addChild ( this->_buildLatSkirt ( _extents.minimum()[1], _texCoords[2], 0,                  offset, ll ) ); // Bottom skirt.
    mt->addChild ( this->_buildLatSkirt ( _extents.maximum()[1], _texCoords[3], mesh.columns() - 1, offset, ll ) ); // Top skirt.
  }

  // Make the ground.
  mt->addChild ( mesh() );

  // Add the place-holder for the border.
  mt->addChild ( _borders.get() );

  // Add the group to us.
  this->addChild ( mt.get() );
  
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
  if ( ( true == _texture.valid() ) && ( 0x0 != _image.get() ) && ( true == this->textureDirty() ) )
  {
    // Set the image.
    _texture->setImage ( _image.get() );
    
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
//  Traverse the children.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::traverse ( osg::NodeVisitor &nv )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Launch the image request if one is needed.
  if ( ( Usul::Bits::has ( _flags, Tile::IMAGE ) ) && ( false == _imageJob.valid() ) )
  {
    this->_launchImageRequest();
  }

  if ( Usul::Bits::has ( _flags, Tile::CHILDREN ) )
  {
    // Clear all the children.
    this->_clearChildren();
    this->dirty ( false, Tile::CHILDREN, false );

    this->dirtyBound();
  }

  // If it's a cull visitor...
  if ( osg::NodeVisitor::CULL_VISITOR == nv.getVisitorType() )
  {
    // See if our job is done loading image.
    if ( _imageJob.valid() && _imageJob->isDone() )
    {
      osg::ref_ptr < osg::Image > image ( _imageJob->image() );
      if ( image.valid() )
      {
        this->textureData ( image.get(), Usul::Math::Vec4d ( 0.0, 1.0, 0.0, 1.0 ) );
        _imageJob = 0x0;
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
    if ( this->verticesDirty() )
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
  Mesh &mesh ( *_mesh );
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

  // Check to see if eye is zero.
  const bool eyeIsZero ( 0.0 == eye[0] || 0.0 == eye[1] || 0.0 == eye[2] );

  // Check if we've gone too deep.
  const bool tooDeep ( this->level() >= _body->maxLevel() );

  // Should we traverse the low lod?
  bool low ( farAway || eyeIsNan || eyeIsZero || tooDeep );

  // Finally, ask the callback.
  low = !( _body->shouldSplit ( !low, this ) );

  if ( low )
  {    
    this->updateTexture();

    // Turn off the borders.
    OsgTools::Group::removeAllChildren ( _borders.get() );

    // Remove high level of detail.
    if ( numChildren > 1 )
    {
      if ( false == _body->cacheTiles() )
      {
        // Clear all the children.
        this->_clearChildren();
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

          // Show the border.
          OsgTools::Group::removeAllChildren ( _borders.get() );
          _borders->addChild ( this->_buildBorderLine().get() );
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

        // Turn off the borders.
        OsgTools::Group::removeAllChildren ( _borders.get() );
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
  
  // Width and height for the image.
  const unsigned int width ( 512 );
  const unsigned int height ( 512 );
  
  // Raster data.
  RasterLayer::RefPtr raster ( body->rasterData() );
  
  // Build the texture for the tile.
  osg::ref_ptr < osg::Image > image ( Tile::buildRaster ( extents, width, height, level, raster.get(), job ) );
  
  // If we didn't get an image, use our image.
  if ( false == image.valid() )
  {
    image = Usul::Threads::Safe::get ( this->mutex(), _image.get() );
    
    // Use the suggested texture coordinates.
    tCoords.set ( texCoords[0], texCoords[1], texCoords[2], texCoords[3] );
  }
  
  // Elevation data.
  RasterLayer::RefPtr elevationData ( body->elevationData() );
  
  // Get the data for our elevation.
  osg::ref_ptr < osg::Image > elevation ( Tile::buildRaster ( extents, size[0], size[1], level, elevationData.get(), job ) );
  
  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();
  
  // Make the tile.
  Tile::RefPtr tile ( new Tile ( level, extents, size, tCoords, splitDistance, body.get(), image.get(), elevation.get() ) );
  tile->updateMesh();
  tile->updateTexture();
  
  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();
  
  return tile;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build texture for this tile.
//
///////////////////////////////////////////////////////////////////////////////

Tile::ImagePtr Tile::buildRaster ( const Extents &extents, unsigned int width, unsigned int height, unsigned int level, RasterLayer* raster, Usul::Jobs::Job::RefPtr job )
{
  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();
  
  osg::ref_ptr < osg::Image > image ( 0x0 );

  // Build the image.
  if ( 0x0 != raster )
    image = raster->texture ( extents, width, height, level, job, 0x0 );
  
  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();
  
  return image;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the line-segment for the border.
//
///////////////////////////////////////////////////////////////////////////////

Tile::NodePtr Tile::_buildBorderLine()
{
  // Make geode to hold the geometry.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );

  // If we have a mesh...
  if ( 0x0 != _mesh )
  {
    // Shortcut.
    Mesh &mesh ( *_mesh );

    // Make the colors.
    osg::Vec4f color ( 1.0f, 0.0f, 0.0f, 1.0f );
    osg::ref_ptr<osg::Vec4Array> horizColors ( new osg::Vec4Array );
    osg::ref_ptr<osg::Vec4Array> vertColors  ( new osg::Vec4Array );
    horizColors->resize ( mesh.columns(), color );
    vertColors->resize  ( mesh.rows(),    color );

    // Make the vertices.
    osg::ref_ptr<osg::Vec3Array> top    ( new osg::Vec3Array );
    osg::ref_ptr<osg::Vec3Array> bottom ( new osg::Vec3Array );
    osg::ref_ptr<osg::Vec3Array> left   ( new osg::Vec3Array );
    osg::ref_ptr<osg::Vec3Array> right  ( new osg::Vec3Array );

    // Make space.
    top->reserve    ( mesh.columns() );
    bottom->reserve ( mesh.columns() );
    left->reserve   ( mesh.rows() );
    right->reserve  ( mesh.rows() );

    // Loop through the columns.
    for ( unsigned int j = 0; j < mesh.columns(); ++j )
    {
      top->push_back    ( mesh.point (               0, j ) );
      bottom->push_back ( mesh.point ( mesh.rows() - 1, j ) );
    }

    // Loop through the rows.
    for ( unsigned int i = 0; i < mesh.rows(); ++i )
    {
      left->push_back  ( mesh.point ( i, 0 ) );
      right->push_back ( mesh.point ( i, mesh.columns() - 1 ) );
    }

    // Top
    {
      // Make the geometry.
      osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );

      // Add geometry properties.
      geometry->setVertexArray ( top.get() );
      geometry->setColorArray ( horizColors.get() );
      geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
      geometry->setNormalArray ( 0x0 );
      geometry->setNormalBinding ( osg::Geometry::BIND_OFF );
      geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINE_STRIP, 0, geometry->getVertexArray()->getNumElements() ) );

      // Add geometry to geode.
      geode->addDrawable ( geometry.get() );
    }

    // Bottom
    {
      // Make the geometry.
      osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );

      // Add geometry properties.
      geometry->setVertexArray ( bottom.get() );
      geometry->setColorArray ( horizColors.get() );
      geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
      geometry->setNormalArray ( 0x0 );
      geometry->setNormalBinding ( osg::Geometry::BIND_OFF );
      geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINE_STRIP, 0, geometry->getVertexArray()->getNumElements() ) );

      // Add geometry to geode.
      geode->addDrawable ( geometry.get() );
    }

    // Left
    {
      // Make the geometry.
      osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );

      // Add geometry properties.
      geometry->setVertexArray ( left.get() );
      geometry->setColorArray ( vertColors.get() );
      geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
      geometry->setNormalArray ( 0x0 );
      geometry->setNormalBinding ( osg::Geometry::BIND_OFF );
      geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINE_STRIP, 0, geometry->getVertexArray()->getNumElements() ) );

      // Add geometry to geode.
      geode->addDrawable ( geometry.get() );
    }

    // Right
    {
      // Make the geometry.
      osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );

      // Add geometry properties.
      geometry->setVertexArray ( right.get() );
      geometry->setColorArray ( vertColors.get() );
      geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
      geometry->setNormalArray ( 0x0 );
      geometry->setNormalBinding ( osg::Geometry::BIND_OFF );
      geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINE_STRIP, 0, geometry->getVertexArray()->getNumElements() ) );

      // Add geometry to geode.
      geode->addDrawable ( geometry.get() );
    }
  }

  // Set properties.
  OsgTools::State::StateSet::setLineWidth ( geode.get(), 3.0f );
  OsgTools::State::StateSet::setPolygonsTextures ( geode->getOrCreateStateSet(), false );

  // Need an offset.
  osg::ref_ptr<osg::PolygonOffset> offset ( new osg::PolygonOffset );
  offset->setFactor ( -1.0f );
  offset->setUnits  ( -1.0f );
  geode->getOrCreateStateSet()->setAttributeAndModes ( offset.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );

  // Return new geode.
  return NodePtr ( geode.get() );
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

osg::Node* Tile::_buildLonSkirt ( double lon, double u, unsigned int i, double offset, const Mesh::Vector& ll )
{
  const unsigned int columns ( _mesh->columns() );

  // Make the mesh
  Mesh mesh ( 2, columns );

  // Left skirt.
  for ( unsigned int j = 0; j < columns; ++j )
  {
    const double v ( static_cast<double> ( j ) / ( columns - 1 ) );
    const double lat ( _extents.minimum()[1] + v * ( _extents.maximum()[1] - _extents.minimum()[1] ) );

    const double elevation ( ( _elevation.valid() ? ( *reinterpret_cast < const float * > ( _elevation->data ( i, j ) ) ) : 0.0 ) );

    Mesh::Vector&  p0 ( mesh.point ( 0, j ) );
    Mesh::Vector&  p1 ( mesh.point ( 1, j ) );
    
    // Convert lat-lon coordinates to xyz.
    _body->latLonHeightToXYZ ( lat, lon, elevation,          p0 );
    _body->latLonHeightToXYZ ( lat, lon, elevation - offset, p1 );
    
    p0 = p0 - ll;
    p1 = p1 - ll;

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

osg::Node* Tile::_buildLatSkirt ( double lat, double v, unsigned int j, double offset, const Mesh::Vector& ll )
{
  const unsigned int rows ( _mesh->rows() );

  // Make the mesh
  Mesh mesh ( rows, 2 );

  // Left skirt.
  for ( int i = rows - 1; i >= 0; --i )
  {
    const double u ( 1.0 - static_cast<double> ( i ) / ( mesh.rows() - 1 ) );
    const double lon ( _extents.minimum()[0] + u * ( _extents.maximum()[0] - _extents.minimum()[0] ) );

    const double elevation ( ( _elevation.valid() ? ( *reinterpret_cast < const float * > ( _elevation->data ( i, j ) ) ) : 0.0 ) );

    Mesh::Vector&  p0 ( mesh.point ( i, 0 ) );
    Mesh::Vector&  p1 ( mesh.point ( i, 1 ) );
    
    // Convert lat-lon coordinates to xyz.
    _body->latLonHeightToXYZ ( lat, lon, elevation,          p0 );
    _body->latLonHeightToXYZ ( lat, lon, elevation - offset, p1 );
    
    p0 = p0 - ll;
    p1 = p1 - ll;

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

  // Cancel job if it's valid.
  if ( true == _imageJob.valid() )
  {
    Guard guard ( this );
    _imageJob->cancel();
    _imageJob = 0x0;
  }

  {
    Guard guard ( this );
    if ( _tileJob.valid() )
    {
      _tileJob->cancel();
      _tileJob = 0x0;
    }
  }

  this->dirty ( false, Tile::ALL, false );
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
    if ( true == _imageJob.valid() )
    {
      _imageJob->cancel();
      _imageJob = 0x0;
    }

    _imageJob = _body->textureRequest ( this->extents(), this->level() );
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
    Usul::Interfaces::IUnknown::RefPtr caller ( 0x0 );
    _elevationJob = new CutImageJob ( this->extents(), size[0], size[1], this->level(), _body->elevationData(), caller );
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
  
  // Our image is no longer dirty.
  this->dirty ( false, Tile::IMAGE, false );
  
  // Our texture needs to be updated.
  this->dirty ( true, Tile::TEXTURE, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image.
//
///////////////////////////////////////////////////////////////////////////////

Tile::ImagePtr Tile::image()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _image;
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

void Tile::textureData ( osg::Image* image, const Usul::Math::Vec4d& coords )
{
  USUL_TRACE_SCOPE;

  // Set the texture coordinates.
  this->texCoords ( coords );
  
  // Set children data.
  {
    Guard guard ( this );

    Usul::Math::Vec4d ll, lr, ul, ur;
    this->_quarterTextureCoordinates ( ll, lr, ul, ur );
    if ( _children[LOWER_LEFT].valid()  && image == _children[LOWER_LEFT]->image()  ) _children[LOWER_LEFT]->textureData  ( image, ll );
    if ( _children[LOWER_RIGHT].valid() && image == _children[LOWER_RIGHT]->image() ) _children[LOWER_RIGHT]->textureData ( image, lr );
    if ( _children[UPPER_LEFT].valid()  && image == _children[UPPER_LEFT]->image()  ) _children[UPPER_LEFT]->textureData  ( image, ul );
    if ( _children[UPPER_RIGHT].valid() && image == _children[UPPER_RIGHT]->image() ) _children[UPPER_RIGHT]->textureData ( image, ur );
  }

  // Set our image.
  this->image ( image );
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
  //return BaseClass::computeBound();
  return _boundingSphere;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear children.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_clearChildren()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  this->removeChild ( 1, this->getNumChildren() - 1 );

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
  if ( _tileJob.valid() )
  {
    _tileJob->cancel();
    _tileJob = 0x0;
  }
}
