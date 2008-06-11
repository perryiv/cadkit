
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

#include "Minerva/Core/TileEngine/Tile.h"
#include "Minerva/Core/TileEngine/Body.h"
#include "Minerva/Core/Layers/Container.h"
#include "Minerva/Core/Layers/RasterLayerGDAL.h"
#include "Minerva/Core/Jobs/BuildTiles.h"
#include "Minerva/Core/Utilities/Composite.h"
#include "Minerva/Core/Utilities/SubRegion.h"

#include "OsgTools/Group.h"
#include "OsgTools/Utilities/FindNormals.h"
#include "OsgTools/State/StateSet.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Bind.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Functions/Execute.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Math/NaN.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Jobs/Manager.h"

#include "osgUtil/CullVisitor"

#include "osg/BlendFunc"
#include "osg/Depth"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Hint"
#include "osg/PolygonOffset"
#include "osg/Material"
#include "osg/Texture2D"

#include "osgDB/ReadFile"

#include "boost/bind.hpp"

#include <algorithm>
#include <limits>

#define USE_MIP_MAPS 1

using namespace Minerva::Core::TileEngine;

USUL_IMPLEMENT_TYPE_ID ( Tile );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Tile::Tile ( Tile* parent, Indices index, unsigned int level, const Extents &extents, 
             const MeshSize &meshSize, const ImageSize& imageSize, double splitDistance, 
             Body *body, osg::Image* image, osg::Image * elevation ) : 
  BaseClass(),
  _mutex ( new Tile::Mutex ),
  _body ( body ),
  _extents ( extents ),
  _splitDistance ( splitDistance ),
  _mesh ( new Mesh ( meshSize[0], meshSize[1] ) ),
  _lowerLeft(),
  _level ( level ),
  _flags ( Tile::ALL ),
  _children ( 4 ),
  _image ( image ),
  _elevation ( elevation ),
  _texture ( new osg::Texture2D ),
  _texCoords ( 0.0, 1.0, 0.0, 1.0 ),
  _imageJob ( 0x0 ),
  _elevationJob ( 0x0 ),
  _tileJob ( 0x0 ),
  _boundingSphere(),
  _borders ( new osg::Group ),
  _skirts ( new osg::Group ),
  _textureMap (),
  _imageSize ( imageSize ),
  _parent ( parent ),
  _index ( index )
{
  USUL_TRACE_SCOPE;

  // We want thread safe ref and unref.
  this->setThreadSafeRefUnref ( true );

  // Set the image.
  if ( true == _image.valid() )
    _texture->setImage ( _image.get() );
  
#if USE_MIP_MAPS == 1
  // Set filter parameters.
  _texture->setFilter ( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_NEAREST );
  _texture->setFilter ( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
  
  _texture->setUseHardwareMipMapGeneration ( true );
  
  // Set texture coordinate wrapping parameters.
  _texture->setWrap ( osg::Texture::WRAP_S, osg::Texture::MIRROR );
  _texture->setWrap ( osg::Texture::WRAP_T, osg::Texture::MIRROR );
#else
  // Set filter parameters.
  _texture->setFilter ( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
  _texture->setFilter ( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
  
  // Set texture coordinate wrapping parameters.
  _texture->setWrap ( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
  _texture->setWrap ( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );
#endif

  // Turn off back-face culling.
  this->getOrCreateStateSet()->setMode ( GL_CULL_FACE, osg::StateAttribute::OFF );

   // Get the state set.
  osg::ref_ptr<osg::StateSet > ss ( this->getOrCreateStateSet() );

  // Need an offset.
  osg::ref_ptr<osg::PolygonOffset> offset ( new osg::PolygonOffset );
  offset->setFactor ( 1.0f );
  offset->setUnits  ( 4.0f );
  ss->setAttributeAndModes ( offset.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );

  // For some reason this is now needed or else we cannot see the images.
  OsgTools::State::StateSet::setMaterialDefault ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Tile::Tile ( const Tile &tile, const osg::CopyOp &option ) : 
  BaseClass ( tile, option ),
  _mutex ( new Tile::Mutex ),
  _body ( tile._body ),
  _extents ( tile._extents ),
  _splitDistance ( tile._splitDistance ),
  _mesh ( new Mesh ( tile._mesh->rows(), tile._mesh->columns() ) ),
  _lowerLeft ( tile._lowerLeft ),
  _level ( tile._level ),
  _flags ( Tile::ALL ),
  _children ( tile._children ),
  _image ( tile._image ),
  _elevation ( tile._elevation ),
  _texCoords ( tile._texCoords ),
  _imageJob ( 0x0 ),
  _elevationJob ( 0x0 ),
  _tileJob ( 0x0 ),
  _boundingSphere ( tile._boundingSphere ),
  _borders ( new osg::Group ),
  _skirts ( new osg::Group ),
  _textureMap ( tile._textureMap ),
  _imageSize ( tile._imageSize ),
  _parent ( tile._parent ),
  _index ( tile._index )
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

  // Clear this tile and all it's children.
  this->clear ( true );

  _body = 0x0; // Don't delete!
  /*delete _mesh; _mesh = 0x0;*/
  delete _mutex; _mutex = 0x0;
  _borders = 0x0;
  _skirts = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the mesh.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::updateMesh()
{
  USUL_TRACE_SCOPE;

  // Handle not being dirty.
  if ( false == this->verticesDirty() || false == this->texCoordsDirty() )
    return;

  // Get needed variables.
  Extents extents ( this->extents() );
  MeshSize size ( this->meshSize() );
  ImagePtr elevation ( 0x0 );
  Body::RefPtr body ( 0x0 );
  Usul::Math::Vec4d texCoords;
  {
    Guard guard ( this->mutex() );
    elevation = _elevation;
    body = _body;
    texCoords = _texCoords;
  }

  // Handle bad state.
  if ( 0x0 == body )
    return;

  // The number of expected bytes for our elevation data.
  const unsigned int expectedBytes ( size[0] * size[1] * sizeof ( float ) );

  // Check to make sure our elevation data is valid.
  const bool elevationValid ( elevation.valid() && 
                              GL_FLOAT == elevation->getDataType() && 
                              expectedBytes == elevation->getImageSizeInBytes() &&
                              size[0] == elevation->s() && 
                              size[1] == elevation->t() );

  // Shortcuts.
  const Extents::Vertex &mn ( extents.minimum() );
  const Extents::Vertex &mx ( extents.maximum() );
  const double deltaU ( texCoords[1] - texCoords[0] );
  const double deltaV ( texCoords[3] - texCoords[2] );

  // Make a new mesh.
  MeshPtr pMesh ( new Mesh ( size[0], size[1] ) );
  Mesh &mesh ( *pMesh );

  // Make a new bounding sphere.
  BSphere boundingSphere;

  // Add internal geometry.
  for ( int i = mesh.rows() - 1; i >= 0; --i )
  {
    const double u ( 1.0 - static_cast<double> ( i ) / ( mesh.rows() - 1 ) );
    for ( unsigned int j = 0; j < mesh.columns(); ++j )
    {
      const double v ( static_cast<double> ( j ) / ( mesh.columns() - 1 ) );
      
      const double lon ( mn[0] + u * ( mx[0] - mn[0] ) );
      const double lat ( mn[1] + v * ( mx[1] - mn[1] ) );

      // Convert lat-lon coordinates to xyz.
      Mesh::Vector &p ( mesh.point ( i, j ) );
        
      // Get the elevation.
      // The osg::Image stores it's data as char*.  However, in this case the data of the image is float.
      // The data function will calculate and return the pointer to the beginning of the float.  The pointer needs to be cast to a float pointer so the proper value is accessed.
      const double heightAboveSeaLevel ( ( elevationValid ? ( *reinterpret_cast < const float * > ( elevation->data ( mesh.rows() - 
i - 1, j ) ) 
) : 0.0 ) );
      body->latLonHeightToXYZ ( lat, lon, heightAboveSeaLevel, p );
        
      // Expand the bounding sphere by the point.
      boundingSphere.expandBy ( p );

      // Assign normal vectors.
      Mesh::Vector &n ( mesh.normal ( i, j ) );
      n = p; // Minus the center, which is (0,0,0).
      n.normalize();

      // Assign texture coordinate.  Lower left corner should be (0,0).
      const double s ( ( texCoords[0] + ( u * deltaU ) ) );
      const double t ( ( texCoords[2] + ( v * deltaV ) ) );
      mesh.texCoord ( i, j ).set ( s, t );
    }
  }

  // Make the skirt's normals.
  Mesh::Vector leftNormal   ( mesh.point ( 0, 0 ) ^ mesh.point ( mesh.rows() - 1, 0 ) );    leftNormal.normalize();
  Mesh::Vector rightNormal  ( mesh.point ( mesh.rows() - 1, 0 ) ^ mesh.point ( 0, 0 ) );    rightNormal.normalize();
  Mesh::Vector topNormal    ( mesh.point ( 0, 0 ) ^ mesh.point ( 0, mesh.columns() - 1 ) ); topNormal.normalize();
  Mesh::Vector bottomNormal ( topNormal );

  // Move mesh vertices to local origin.
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

  // Build skirts.
  // Depth of skirt.  TODO: This function needs to be tweeked.
  const double offset ( Usul::Math::maximum<double> ( ( 5000 - ( this->level() * 150 ) ), ( 10 * std::numeric_limits<double>::epsilon() ) ) );
    
  // Make new skirts.
  osg::ref_ptr<osg::Group> skirts ( new osg::Group );

  // Add skirts to group.
  skirts->addChild ( this->_buildLonSkirt ( _extents.minimum()[0], _texCoords[0], mesh.rows() - 1,    offset, ll, leftNormal   ) ); // Left skirt.
  skirts->addChild ( this->_buildLonSkirt ( _extents.maximum()[0], _texCoords[1], 0,                  offset, ll, rightNormal  ) ); // Right skirt.
  skirts->addChild ( this->_buildLatSkirt ( _extents.minimum()[1], _texCoords[2], 0,                  offset, ll, bottomNormal ) ); // Bottom skirt.
  skirts->addChild ( this->_buildLatSkirt ( _extents.maximum()[1], _texCoords[3], mesh.columns() - 1, offset, ll, topNormal    ) ); // Top skirt.

#if 0

  // Draw skirt's normals.
  osg::ref_ptr<OsgTools::Utilities::FindNormals> visitor ( new OsgTools::Utilities::FindNormals );
  osg::BoundingBox bbox;
  bbox.expandBy ( this->getBound() );
  visitor->boundingBox ( bbox );
  _skirts->accept ( *visitor );
  osg::ref_ptr<osg::Group> ng ( new osg::Group );
  OsgTools::State::StateSet::setLineWidth ( ng.get(), 5 );
  OsgTools::State::StateSet::setLighting ( ng.get(), false );
  ng->addChild ( visitor->normals() );
  _skirts->addChild ( ng.get() );

#endif

  // Add to the matrix transform.
  mt->addChild ( skirts.get() );

  // Make the ground.
  mt->addChild ( mesh() );

  // Add the place-holder for the border.
  mt->addChild ( _borders.get() );

  // Remove all the children.
  this->removeChildren ( 0, this->getNumChildren() );

  // Add the group to us.
  this->addChild ( mt.get() );

  // Set needed variables
  {
    Guard guard ( this->mutex() );
    _lowerLeft = ll;
    _mesh = pMesh;
    _boundingSphere = boundingSphere;
    _skirts = skirts;
  }
  
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

  // Return now if we don't have an image.
  if ( false == this->textureDirty() )
    return;

  // Get needed variables.
  ImagePtr image ( 0x0 );
  osg::ref_ptr < osg::Texture2D > texture ( 0x0 );
  Body::RefPtr body ( 0x0 );
  {
    Guard guard ( this->mutex() );
    image = _image;
    texture = _texture;
    body = _body;
  }
  
  // Set the image.
  if ( ( true == texture.valid() ) )
  {
    // Set the image if not null.
    if ( true == image.valid() )
      texture->setImage ( image.get() );

    // Set the proper texture state. This enables us to get a blank planet.
    const unsigned int flags ( ( ( true == image.valid() ) ? osg::StateAttribute::ON : osg::StateAttribute::OFF ) | osg::StateAttribute::PROTECTED );

    // Get the state set.
    osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );
    ss->setTextureAttributeAndModes ( 0, texture.get(), flags );

    // Turn lighting on or off, depending on if there is an image.
    OsgTools::State::StateSet::setLighting ( this, ( false == image.valid() ) );
    
    // Let the body know we have a new texture.
    if ( 0x0 != body )
    {
      body->needsRedraw ( true );
    }

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

  // Get needed variables.
  unsigned int flags ( 0 );
  Usul::Jobs::Job::RefPtr imageJob ( 0x0 );
  Usul::Jobs::Job::RefPtr elevationJob ( 0x0 );
  Usul::Jobs::Job::RefPtr tileJob ( 0x0 );
  Body::RefPtr body ( 0x0 );
  {
    Guard guard ( this );
    flags = _flags;
    imageJob = _imageJob;
    elevationJob = _elevationJob;
    tileJob = _tileJob;
    body = _body;
  }

  // Launch the image request if one is needed.
  if ( ( Usul::Bits::has ( flags, Tile::IMAGE ) ) && ( false == imageJob.valid() ) )
  {
    this->_launchImageRequest();
  }

  if ( Usul::Bits::has ( flags, Tile::CHILDREN ) )
  {
    // Clear all the children.
    this->_clearChildren ( false, true );
    this->dirty ( false, Tile::CHILDREN, false );

    this->dirtyBound();
  }

  // If it's a cull visitor...
  if ( osg::NodeVisitor::CULL_VISITOR == nv.getVisitorType() )
  {
    // See if our job is done loading image.
    if ( imageJob.valid() && imageJob->isDone() )
    {
      Guard guard ( this );
      _imageJob = 0x0;
      imageJob = 0x0;
    }
    
    // Not currently using this...
#if 0
    // Check for new elevation data.
    if ( elevationJob.valid() && elevationJob->isDone() )
    {
      Guard ( this );
      _elevation = _elevationJob->image();
      _elevationJob = 0x0;
      elevationJob = 0x0;
      
      // Force vertices to be rebuilt.
      _flags = Usul::Bits::set ( _flags, Tile::VERTICES, true );
      flags = _flags;
    }
#endif
    
    // Get cull visitor.
    osgUtil::CullVisitor *cv ( dynamic_cast < osgUtil::CullVisitor * > ( &nv ) );
    
    // Make sure the mesh is updated.
    this->updateMesh();
    
    // Make sure our texture is updated.
    this->updateTexture();
    
    // Return if we are culled.
    if ( 0x0 == cv || cv->isCulled ( *this ) )
      return;

    // See if we should draw skirts and borders.
    _skirts->setNodeMask  ( ( body->useSkirts() ? 0xffffffff : 0x0 ) );
    _borders->setNodeMask ( ( body->useBorders() ? 0xffffffff : 0x0 ) );

    // See what we are allowed to do.
    const bool allowSplit ( _body->allowSplit() );
    const bool keepDetail ( _body->keepDetail() );

    // Do we have a high lod?
    const bool hasDetail ( this->getNumChildren() == 2 );

    // Is this the default behavoir?
    const bool defaultMode ( true == allowSplit && false == keepDetail );

    // Do we need to check if we need more detail?
    const bool checkDetail ( true == allowSplit && true == keepDetail && false == hasDetail );

    // Check if we can split.  Don't freeze if we are waiting for a job.
    const bool splitIfNeeded ( ( false == tileJob.valid() && this->getNumChildren() == 0 ) || true == defaultMode || true == checkDetail );

    if ( false == splitIfNeeded )
    {
      const unsigned int child ( ( false == allowSplit && false == keepDetail ) ? 0 : this->getNumChildren() - 1 );
      this->getChild ( child )->accept ( *cv );
    }
    // Spilt.
    else
    {
      this->_cull ( *cv );
    }
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

  // Get needed variables.
  MeshPtr mesh_;
  Usul::Jobs::Job::RefPtr tileJob ( 0x0 );
  Body::RefPtr body ( 0x0 );
  osg::Vec3d lowerLeft;
  double splitDistance ( 0 );
  {
    Guard guard ( this );
    mesh_ = _mesh;
    tileJob = _tileJob;
    body = _body;
    lowerLeft = _lowerLeft;
    splitDistance = _splitDistance;
  }

  // Handle bad state.
  if ( ( 0x0 == mesh_ ) || 
       ( mesh_->rows() < 2 ) || 
       ( mesh_->columns() < 2 ) ||
       ( 0x0 == body ) ||
       ( 0x0 == body->jobManager() ) )
  {
    return;
  }

  // Four corners and center of the tile.
  Mesh &mesh ( *mesh_ );
  const osg::Vec3f &eye ( cv.getViewPointLocal() );

#if 0
  const osg::Vec3f look ( cv.getLookVectorLocal() );
  
  // Get the normals.
  const Mesh::Vector &n0 ( mesh.normal ( 0, 0 ) );
  const Mesh::Vector &n1 ( mesh.normal ( 0, mesh.columns() - 1 ) );
  const Mesh::Vector &n2 ( mesh.normal ( mesh.rows() - 1, 0 ) );
  const Mesh::Vector &n3 ( mesh.normal ( mesh.rows() - 1, mesh.columns() - 1 ) );
  const Mesh::Vector &n4 ( mesh.normal ( mesh.rows() / 2, mesh.columns() / 2 ) );

  // Get the dot product.  A positive number is facing towards the viewer.
  const double d0 ( n0 * look );
  const double d1 ( n1 * look );
  const double d2 ( n2 * look );
  const double d3 ( n3 * look );
  const double d4 ( n4 * look );
  
  const bool back ( d0 > 0.0 && d1 > 0.0 && d2 > 0.0 && d3 > 0.0 && d4 > 0.0 );
#else
  const bool back ( false );
#endif

  const osg::Vec3f &p00 ( lowerLeft + mesh.point ( 0, 0 ) );
  const osg::Vec3f &p0N ( lowerLeft + mesh.point ( 0, mesh.columns() - 1 ) );
  const osg::Vec3f &pN0 ( lowerLeft + mesh.point ( mesh.rows() - 1, 0 ) );
  const osg::Vec3f &pNN ( lowerLeft + mesh.point ( mesh.rows() - 1, mesh.columns() - 1 ) );
  const osg::Vec3f &pBC ( this->getBound().center() );
  
  // Squared distances from the eye to the points.
  const float dist00 ( ( eye - p00 ).length2() );
  const float dist0N ( ( eye - p0N ).length2() );
  const float distN0 ( ( eye - pN0 ).length2() );
  const float distNN ( ( eye - pNN ).length2() );
  const float distBC ( ( eye - pBC ).length2() );

  // Check with smallest distance.
  const float dist ( Usul::Math::minimum ( dist00, dist0N, distN0, distNN, distBC ) );
  const bool farAway ( ( dist > ( splitDistance * splitDistance ) ) );
  const unsigned int numChildren ( this->getNumChildren() );
  USUL_ASSERT ( numChildren > 0 );

  // Check for nan in eye values (it's happened).
  const bool eyeIsNan ( Usul::Math::nan ( eye[0] ) || Usul::Math::nan ( eye[1] ) || Usul::Math::nan ( eye[2] ) );

  // Check if we've gone too deep.
  const bool tooDeep ( this->level() >= body->maxLevel() );

  // Should we traverse the low lod?
  bool low ( farAway || eyeIsNan || tooDeep || back );

  // Finally, ask the callback.
  low = !( body->shouldSplit ( !low, this ) );
  
  if ( low )
  {
    // Turn off the borders.
    OsgTools::Group::removeAllChildren ( _borders.get() );

    // Remove high level of detail.
    if ( numChildren > 1 && false == body->cacheTiles() )
    {
      // Clear all the children.
      this->_clearChildren ( false, true );
    }
  }

  else
  {
    // Add high level if necessary.
    if ( 1 == numChildren )
    {
      bool hasChildren ( false );
      {
        Guard guard ( this->mutex() );
        hasChildren = ( _children[LOWER_LEFT].valid()  ) &&
                      ( _children[LOWER_RIGHT].valid() ) &&
                      ( _children[UPPER_LEFT].valid()  ) &&
                      ( _children[UPPER_RIGHT].valid() );
      }

      // Make tiles if we are not caching them, or if this is the first time.
      if ( ( false == body->cacheTiles() ) || 
           ( false == hasChildren ) )
      {
        // Use the low lod while we are waiting for the job.
        if ( tileJob.valid() )
        {
          low = true;
        }
        else
        {
          Guard guard ( this->mutex() );

          // Make a new job to tile the child tiles.
          _tileJob = new Minerva::Core::Jobs::BuildTiles ( Tile::RefPtr ( this ) );
        
          // Add the job to the job manager.
          _body->jobManager()->addJob ( _tileJob.get() );

          // Show the border.
          OsgTools::Group::removeAllChildren ( _borders.get() );
          _borders->addChild ( this->_buildBorderLine().get() );
        }
      }

      if ( tileJob.valid() && tileJob->isDone() )
      {
        // Did it work?
        if ( true == tileJob->success() )
        {
          Guard guard ( this->mutex() );

          // Add the children.
          osg::ref_ptr<osg::Group> group ( new osg::Group );
          group->addChild ( _children[LOWER_LEFT]  );
          group->addChild ( _children[LOWER_RIGHT] );
          group->addChild ( _children[UPPER_LEFT]  );
          group->addChild ( _children[UPPER_RIGHT] );
          this->addChild ( group.get() );
        }

        // It did not work.
        else
        {
          // Go low. The next cull will try again unless the user zoomed out.
          low = true;

          // Some of the children may have been created, so clear them all.
          // Do not cancel the job since it already finished.
          this->_clearChildren ( false, false );
        }

        Guard guard ( this->mutex() );

        _tileJob = 0x0;
        tileJob = 0x0;

        // Turn off the borders.
        OsgTools::Group::removeAllChildren ( _borders.get() );
      }
    }
  }
  
  // Traverse low level of detail.
  if ( low || tileJob.valid() )
  {
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

  Tile::RefPtr t0 ( this->_buildTile ( level, ll, mll, tll, half, job, LOWER_LEFT  ) ); // lower left  tile
  Tile::RefPtr t1 ( this->_buildTile ( level, lr, mlr, tlr, half, job, LOWER_RIGHT ) ); // lower right tile
  Tile::RefPtr t2 ( this->_buildTile ( level, ul, mul, tul, half, job, UPPER_LEFT  ) ); // upper left  tile
  Tile::RefPtr t3 ( this->_buildTile ( level, ur, mur, tur, half, job, UPPER_RIGHT ) ); // upper right tile
  
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

  // Need to notify vector data so it can re-adjust.
  Minerva::Core::Layers::Container::RefPtr vector ( body->vectorData() );
  if ( vector.valid() )
  {
    Usul::Interfaces::IUnknown::QueryPtr unknown ( body );
    
    Extents extents0 ( t0->extents() );
    ImagePtr elevation0 ( t0->elevation() );

    // Since elevation data that is passed isn't currently used just ask once.
    vector->elevationChangedNotify ( extents, elevation0, unknown.get() );
    //vector->elevationChangedNotify ( t1->extents(), t1->elevation(), unknown.get() );
    //vector->elevationChangedNotify ( t2->extents(), t2->elevation(), unknown.get() );
    //vector->elevationChangedNotify ( t3->extents(), t3->elevation(), unknown.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the degrees per pixel.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  Usul::Math::Vec2d degreesPerPixel ( const Tile::Extents& extents, const Tile::ImageSize& size )
  {
    typedef Tile::Extents Extents;
    
    const Extents::Vertex &mn ( extents.minimum() );
    const Extents::Vertex &mx ( extents.maximum() );

    return Usul::Math::Vec2d ( ( mx[0] - mn[0] ) / size[0], ( mx[1] - mn[1] ) / size[1] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Expand the extents by the given amount.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  Tile::Extents expandExtents ( const Tile::Extents& extents, const Usul::Math::Vec2d& amount )
  {
    typedef Tile::Extents Extents;
    const Extents::Vertex &mn ( extents.minimum() );
    const Extents::Vertex &mx ( extents.maximum() );

    return Extents ( mn[0] - ( amount[0] / 2.0 ), mn[1] - ( amount[1] / 2.0 ), mx[0] + ( amount[0] / 2.0 ), mx[1] + ( amount[1] / 2.0 ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a tile.
//
///////////////////////////////////////////////////////////////////////////////

Tile::RefPtr Tile::_buildTile ( unsigned int level, 
                                const Extents& extents, 
                                const MeshSize& size, 
                                const Usul::Math::Vec4d& region, 
                                double splitDistance, 
                                Usul::Jobs::Job::RefPtr job,
                                Indices index )
{
  // If our logic is correct, this should be true.
  USUL_ASSERT ( this->referenceCount() >= 1 );
  
  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();

  Body::RefPtr body ( Usul::Threads::Safe::get ( this->mutex(), _body ) );
  
  // Handle no body.
  if ( false == body.valid() )
    return 0x0;

  // Width and height for the image.
  const ImageSize imageSize ( Usul::Threads::Safe::get ( this->mutex(), _imageSize ) );

  // Elevation data.
  Minerva::Core::Layers::RasterLayer::RefPtr elevationData ( body->elevationData() );
  
  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();

  // Expand the request by degreesPerPixel / 2.0 on each side.
  const Extents request ( Helper::expandExtents ( extents, Helper::degreesPerPixel ( extents, size ) ) );

  // Get the data for our elevation.
  osg::ref_ptr < osg::Image > elevation ( Tile::buildRaster ( request, size[0], size[1], level, elevationData.get(), job ) );
  
  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();

  // See if we have valid elevation.
  if ( false == elevation.valid() )
  {
    // Use a quarter of the parent's elevation for the child.
    osg::ref_ptr<osg::Image> parentElevation ( Usul::Threads::Safe::get ( this->mutex(), _elevation ) );
    if ( parentElevation.valid() )
    {
#if 0
      Extents parentExtents ( Helper::expandExtents ( this->extents(), Helper::degreesPerPixel ( this->extents(), size ) ) );

      typedef Minerva::Core::Layers::RasterLayerGDAL Resampler;
      Resampler::RefPtr layer ( new Resampler ( parentElevation.get(), parentExtents ) );
      elevation = layer->texture ( request, size[0], size[1], level, job, 0x0 );
#else
      elevation = Minerva::Core::Utilities::subRegion<float> ( *parentElevation, region, GL_LUMINANCE, GL_FLOAT );
#endif
    }
  }
  
  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();
  
  // Make the tile.
  Tile::RefPtr tile ( new Tile ( this, index, level, extents, size, imageSize, splitDistance, body.get(), 0x0, elevation.get() ) );

  // Build the raster.  Make sure this is done before mesh is built and texture updated.
  tile->buildRaster ( job );

  // Check to see if the tile has a valid image.
  if ( false == tile->image().valid() && 0x0 != this->image() )
  {
    // Use the specified region of our image.
    tile->textureData ( this->image().get(), region );
  }
  
  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();

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

Tile::ImagePtr Tile::buildRaster ( const Extents &extents, unsigned int width, unsigned int height, unsigned int level, IRasterLayer* raster, Usul::Jobs::Job::RefPtr job )
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
//  Build raster.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::buildRaster ( Usul::Jobs::Job::RefPtr job )
{
  // Get the parent.
  Tile::RefPtr parent ( Usul::Threads::Safe::get ( this->mutex(), _parent.get() ) );
  
  // Get my index.
  Indices index ( Usul::Threads::Safe::get ( this->mutex(), _index ) );
  
  // Get the body.
  Body::RefPtr body ( Usul::Threads::Safe::get ( this->mutex(), _body ) );
  
  // Handle no body.
  if ( false == body.valid() )
    return;

  // Width and height for the image.
  const ImageSize imageSize ( Usul::Threads::Safe::get ( this->mutex(), _imageSize ) );
  const unsigned int width ( imageSize[0] );
  const unsigned int height ( imageSize[1] );

  // Get the rasters.
  typedef Body::Rasters Rasters;
  Rasters rasters;
  body->rasters ( rasters );

  // Image.
  osg::ref_ptr<osg::Image> result ( 0x0 );

  // Build the list of images to be composited.
  for ( Rasters::iterator iter = rasters.begin(); iter != rasters.end(); ++iter )
  {
    // Have we been cancelled?
    if ( ( 0x0 != job ) && ( true == job->canceled() ) )
      job->cancel();

    // The layer.
    IRasterLayer::RefPtr raster ( *iter );

    // Image for the layer.
    osg::ref_ptr<osg::Image> image ( 0x0 );

    // The texture coordinates to use.
    Usul::Math::Vec4d tCoords ( 0.0, 1.0, 0.0, 1.0 );

    // Query for needed interfaces.
    Usul::Interfaces::ILayer::QueryPtr layer ( raster );
    Usul::Interfaces::ILayerExtents::QueryPtr le ( layer );

    // Get the extents.
    const double minLon ( le.valid() ? le->minLon() : -180.0 );
    const double minLat ( le.valid() ? le->minLat() :  -90.0 );
    const double maxLon ( le.valid() ? le->maxLon() :  180.0 );
    const double maxLat ( le.valid() ? le->maxLat() :   90.0 );

    Extents e ( minLon, minLat, maxLon, maxLat );

    // Should the layer be shown?
    const bool shown ( layer.valid() ? layer->showLayer() : true );

    // Only use this layer if it's shown and intersects our extents.
    if ( shown && this->extents().intersects ( e ) )
    {
      // Get the image for the layer.
      image = Tile::buildRaster ( this->extents(), width, height, this->level(), raster.get(), job.get() );

      // If we didn't get an image, use our parents image.
      if ( false == image.valid() )
      {
        TextureData data ( parent.valid() ? parent->texture ( raster.get() ) : TextureData ( 0x0, tCoords ) );
        image = data.first;

        // Set the new texture coordinates.
        tCoords = Tile::_textureCoordinatesSubRegion ( data.second, index );
      }
    }

    // Composite if it's valid...
    if ( true == image.valid() )
    {

      // Save this mapping...
      this->_cacheImage ( raster, image.get(), tCoords );

      // Is this the first image?
      if ( false == result.valid() )
      {
        // We always make an image and composite to handle formats other than GL_RGBA.
        result = new osg::Image;
        result->allocateImage ( width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE );
        ::memset ( result->data(), 0, result->getImageSizeInBytes() );
      }

      // See if it has raster alpha data.
      Usul::Interfaces::IRasterAlphas::QueryPtr ra ( raster );
      
      // Copy the alphas.
      typedef Usul::Interfaces::IRasterAlphas::Alphas Alphas;
      Alphas alphas;
      float alpha ( 1.0f );
      if ( true == ra.valid() )
      {
        alphas = ra->alphas();
        alpha = ra->alpha();
      }
      
      // Composite.
      Minerva::Core::Utilities::Composite::raster ( *result, *image, tCoords, alphas, alpha, 1.0f, job );
    }
  }

  // Always set our image, even if it's null.
  this->textureData ( result.get(), Usul::Math::Vec4d ( 0.0, 1.0, 0.0, 1.0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image from the given RasterLayer.  May return null.
//
///////////////////////////////////////////////////////////////////////////////

Tile::TextureData Tile::texture ( IRasterLayer * layer ) const
{
  Guard guard ( this );
  ImageCache::const_iterator iter ( _textureMap.find ( IRasterLayer::RefPtr ( layer ) ) );
  if ( iter != _textureMap.end() )
  {
    std::string filename ( iter->second.first );
    return TextureData ( osgDB::readImageFile ( filename ), iter->second.second );
  }
  return TextureData ( 0x0, Usul::Math::Vec4d ( 0.0, 1.0, 0.0, 1.0 ) );
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
  if ( 0x0 != _mesh.get() )
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
  OsgTools::State::StateSet::setLighting ( geode.get(), false );
  
  // Get the state set.
  osg::ref_ptr<osg::StateSet > ss ( geode->getOrCreateStateSet() );
  
  // Set depth parameters.
  osg::ref_ptr<osg::Depth> depth ( new osg::Depth ( osg::Depth::LEQUAL, 0.0, 1.0, false ) );
  ss->setAttributeAndModes ( depth.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
  
  // Set the line parameters.
  ss->setMode ( GL_LINE_SMOOTH, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
  ss->setMode ( GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
  
  // Add a blend function.
  osg::ref_ptr<osg::BlendFunc> blend ( new osg::BlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );
  ss->setAttributeAndModes ( blend.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
 
  // Set the hint.
  osg::ref_ptr<osg::Hint> hint ( new osg::Hint ( GL_LINE_SMOOTH_HINT, GL_NICEST ) );
  ss->setAttributeAndModes ( hint.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );

  // Draw after the tile.
  ss->setRenderBinDetails ( 2, "RenderBin" );

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
  this->_setDirtyAlways ( state, flags, dirtyChildren );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Mark the dirty state, only if we cross this extents.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::dirty ( bool state, unsigned int flags, bool dirtyChildren, const Extents& extents )
{
  this->_setDirtyIfIntersect ( state, flags, dirtyChildren, extents );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell the body it can delete us.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_deleteMe()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Add this tile to the body.
  if ( 0x0 != _body )
  {
    _body->_addTileToBeDeleted ( this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag that says we're dirty.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_setDirtyAlways ( bool state, unsigned int flags, bool dirtyChildren )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _flags = Usul::Bits::set ( _flags, flags, state );

  if ( dirtyChildren )
  {
    Usul::Functions::executeMemberFunctions ( _children, &Tile::_setDirtyAlways, state, flags, dirtyChildren );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Mark the dirty state, only if we cross this extents.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_setDirtyIfIntersect ( bool state, unsigned int flags, bool dirtyChildren, const Extents& extents )
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
      Usul::Functions::executeMemberFunctions ( _children, &Tile::_setDirtyIfIntersect, state, flags, dirtyChildren, extents );
    }
  }
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

osg::Node* Tile::_buildLonSkirt ( double lon, double u, unsigned int i, double offset, const Mesh::Vector& ll, const Mesh::Vector &normal )
{
  const unsigned int rows ( _mesh->rows() );
  const unsigned int columns ( _mesh->columns() );

  // Make the mesh
  Mesh mesh ( 2, columns );
  
  // Get the difference of texture coordinates.
  const double difference ( _texCoords[3] - _texCoords[2] );

  // Left skirt.
  for ( unsigned int j = 0; j < columns; ++j )
  {
    const double v ( static_cast<double> ( j ) / ( columns - 1 ) );
    const double lat ( _extents.minimum()[1] + v * ( _extents.maximum()[1] - _extents.minimum()[1] ) );

    const double elevation ( ( _elevation.valid() ? ( *reinterpret_cast < const float * > ( _elevation->data ( rows - i - 1, j ) ) ) : 0.0 ) );

    Mesh::Vector&  p0 ( mesh.point ( 0, j ) );
    Mesh::Vector&  p1 ( mesh.point ( 1, j ) );
    
    // Convert lat-lon coordinates to xyz.
    _body->latLonHeightToXYZ ( lat, lon, elevation,          p0 );
    _body->latLonHeightToXYZ ( lat, lon, elevation - offset, p1 );
    
    p0 = p0 - ll;
    p1 = p1 - ll;

    // Assign texture coordinate.
    mesh.texCoord ( 0, j ).set ( u, _texCoords[2] + ( v * difference ) );
    mesh.texCoord ( 1, j ).set ( u, _texCoords[2] + ( v * difference ) );

    // Assign normal vector.
    mesh.normal ( 0, j ) = normal;
    mesh.normal ( 1, j ) = normal;
  }

  return mesh();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build skirt along latitude line.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Tile::_buildLatSkirt ( double lat, double v, unsigned int j, double offset, const Mesh::Vector& ll, const Mesh::Vector &normal )
{
  const unsigned int rows ( _mesh->rows() );

  // Make the mesh
  Mesh mesh ( rows, 2 );
  
  // Get the difference of texture coordinates.
  const double difference ( _texCoords[3] - _texCoords[2] );

  // Left skirt.
  for ( int i = rows - 1; i >= 0; --i )
  {
    const double u ( 1.0 - static_cast<double> ( i ) / ( mesh.rows() - 1 ) );
    const double lon ( _extents.minimum()[0] + u * ( _extents.maximum()[0] - _extents.minimum()[0] ) );

    const double elevation ( ( _elevation.valid() ? ( *reinterpret_cast < const float * > ( _elevation->data ( mesh.rows() - i - 1, j ) ) ) : 0.0 ) );

    Mesh::Vector&  p0 ( mesh.point ( i, 0 ) );
    Mesh::Vector&  p1 ( mesh.point ( i, 1 ) );
    
    // Convert lat-lon coordinates to xyz.
    _body->latLonHeightToXYZ ( lat, lon, elevation,          p0 );
    _body->latLonHeightToXYZ ( lat, lon, elevation - offset, p1 );
    
    p0 = p0 - ll;
    p1 = p1 - ll;

    // Assign texture coordinate.
    mesh.texCoord ( i, 0 ).set ( _texCoords[0] + ( u * difference ), v );
    mesh.texCoord ( i, 1 ).set ( _texCoords[0] + ( u * difference ), v );

    // Assign normal vector.
    mesh.normal ( i, 0 ) = normal;
    mesh.normal ( i, 1 ) = normal;
  }

  return mesh();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::clear ( bool children )
{
  USUL_TRACE_SCOPE;

  // Clear children first if we should.
  if ( true == children )
  {
    this->_clearChildren ( children, true );
  }

  // Cancel job if it's valid.
  {
    Guard guard ( this );
    if ( true == _imageJob.valid() )
    {
      _imageJob->cancel();
      _imageJob = 0x0;
    }
  }

  // Cancel job if it's valid.
  {
    Guard guard ( this );
    if ( true == _tileJob.valid() )
    {
      _tileJob->cancel();
      _tileJob = 0x0;
    }
  }

  // Set the body to null. We have to do this because the tiles 
  // in jobs may live longer than the body.
  _body = 0x0;

  // Set dirty flags.
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

  // Cancel the one we have, if any.
  if ( true == _imageJob.valid() )
  {
    _imageJob->cancel();
    _imageJob = 0x0;
  }
  
  // Start the request to pull in texture.
  if ( 0x0 != _body )
  {
    _imageJob = _body->textureRequest ( this );
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

  // Need to revisit this...
#if 0
  // Start the request to pull in texture.
  if ( ( 0x0 != _body ) && ( 0x0 != _body->jobManager() ) )
  {
    MeshSize size ( this->meshSize() );
    Usul::Interfaces::IUnknown::RefPtr caller ( 0x0 );
    _elevationJob = new CutImageJob ( this->extents(), size[0], size[1], this->level(), _body->elevationData(), caller );
    _body->jobManager()->addJob ( _elevationJob.get() );
  }
#endif
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
//  Get the proper sub region of texture coordinates..
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec4d Tile::_textureCoordinatesSubRegion ( const Usul::Math::Vec4d& region, Indices index )
{
  USUL_TRACE_SCOPE_STATIC;

  const double deltaU ( region[1] - region[0] );
  const double deltaV ( region[3] - region[2] );

  const double startU ( region[0] );
  const double halfU  ( region[0] + ( deltaU / 2.0 ) );
  const double endU   ( startU + deltaU );

  const double startV ( region[2] );
  const double halfV  ( region[2] + ( deltaV / 2.0 ) );
  const double endV   ( startV + deltaV );

  switch ( index )
  {
  case LOWER_LEFT:  return Usul::Math::Vec4d ( startU, halfU, startV, halfV );
  case LOWER_RIGHT: return Usul::Math::Vec4d ( halfU,  endU,  startV, halfV );
  case UPPER_LEFT:  return Usul::Math::Vec4d ( startU, halfU, halfV,  endV );
  case UPPER_RIGHT: return Usul::Math::Vec4d ( halfU,  endU,  halfV,  endV );
  }

  return Usul::Math::Vec4d ( 0.0, 1.0, 0.0, 1.0 );
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
  if ( 0x0 != _mesh.get() )
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


///////////////////////////////////////////////////////////////////////////////
//
//  Clear children.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_clearChildren ( bool traverse, bool cancelJob )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  this->removeChild ( 1, this->getNumChildren() - 1 );

  // Clear all the children.
  Usul::Functions::executeMemberFunctions ( _children, &Tile::clear, traverse );

  // Mark the children as ready to be deleted.
  Usul::Functions::executeMemberFunctions ( _children, &Tile::_deleteMe );

  // Assign children to null.
  _children.assign ( _children.size(), 0x0 );

  // Clear the tile job.
  if ( ( true == _tileJob.valid() ) && ( true == cancelJob ) )
  {
    // Without this call to cancel we always have to wait for the job to 
    // finish, even if we've already chosen to go down the low path, which 
    // happens when we are zoomed in and then "view all".
    _tileJob->cancel();
    _tileJob = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the index of the child.
//
///////////////////////////////////////////////////////////////////////////////

Tile::Indices Tile::child ( Tile* tile ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  Tiles::const_iterator iter ( std::find_if ( _children.begin(), _children.end(), Tile::RefPtr::IsEqual ( tile ) ) );
  return static_cast<Indices> ( std::distance<Tiles::const_iterator> ( _children.begin(), iter ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the split distance.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::splitDistance ( double distance, bool children )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _splitDistance = distance;
  
  // Set the children's split distance if we should
  if ( children )
  {
    const double childDistance ( distance / 2.0 );
    if ( _children[0].valid() ) _children[0]->splitDistance ( childDistance, children );
    if ( _children[1].valid() ) _children[1]->splitDistance ( childDistance, children );
    if ( _children[2].valid() ) _children[2]->splitDistance ( childDistance, children );
    if ( _children[3].valid() ) _children[3]->splitDistance ( childDistance, children );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cache the image used for the raster layer.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_cacheImage ( IRasterLayer::RefPtr raster, osg::Image* image, const Usul::Math::Vec4d& tCoords )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  if ( 0x0 != image )
  {
    std::string filename ( image->getFileName() );
    
    if ( false == filename.empty() )
      _textureMap[raster] = ImageCacheData ( filename, tCoords );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this tile a leaf?
//
///////////////////////////////////////////////////////////////////////////////

bool Tile::isLeaf() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return !( _children[0].valid() && _children[1].valid() && _children[2].valid() && _children[3].valid() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child at index i.
//
///////////////////////////////////////////////////////////////////////////////

Tile::RefPtr Tile::childAt ( unsigned int i ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( i < _children.size() ? _children[i] : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation data.
//
///////////////////////////////////////////////////////////////////////////////

Tile::ImagePtr Tile::elevation()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _elevation;
}
