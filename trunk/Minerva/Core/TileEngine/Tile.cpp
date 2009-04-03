
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

#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#endif

#include "Minerva/Core/TileEngine/Tile.h"
#include "Minerva/Core/TileEngine/Body.h"
#include "Minerva/Core/Data/Container.h"
#include "Minerva/Core/Jobs/BuildElevation.h"
#include "Minerva/Core/Jobs/BuildTiles.h"
#include "Minerva/Core/Algorithms/Composite.h"
#include "Minerva/Core/Algorithms/Split.h"
#include "Minerva/Core/Algorithms/SubRegion.h"
#include "Minerva/Core/Algorithms/ResampleElevation.h"

#include "OsgTools/Group.h"
#include "OsgTools/Utilities/FindNormals.h"
#include "OsgTools/State/StateSet.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Bind.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Functions/Execute.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/ITileVectorJob.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Math/NaN.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Jobs/Manager.h"

#include "osgUtil/CullVisitor"

#include "osg/Material"
#include "osg/Texture2D"

#include "osgDB/ReadFile"

#include "boost/bind.hpp"

#include <algorithm>
#include <limits>

#define USE_MIP_MAPS 1
#define USE_TOP_DOWN_BUILD_RASTER 1 // A value of 1 is the older way of building raster data.

using namespace Minerva::Core::TileEngine;

USUL_IMPLEMENT_TYPE_ID ( Tile );


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to remove job from queue and cancel it.
//  Make sure needed mutexes are locked before calling.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void removeAndCancelJob ( Body* body, Usul::Jobs::Job::RefPtr& job )
  {
    if ( true == job.valid() )
    {
      // Remove the job from the queue.
      if ( 0x0 != body && 0x0 != body->jobManager() )
        body->jobManager()->removeQueuedJob ( job );
      
      // Cancel the job and set to null.
      job->cancel();
      job = 0x0;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Tile::Tile ( Tile* parent, Indices index, unsigned int level, const Extents &extents, 
             const MeshSize &meshSize, const ImageSize& imageSize, double splitDistance, 
             Body *body, osg::Image* image, ElevationDataPtr elevation,
             TileVectorData::RefPtr tileVectorData ) : 
  BaseClass(),
  _mutex ( new Tile::Mutex ),
  _body ( body ),
  _extents ( extents ),
  _splitDistance ( splitDistance ),
  _meshSize ( meshSize ),
  _mesh ( MeshPtr ( static_cast<Mesh*> ( 0x0 ) ) ),
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
  _vector ( new osg::Group ),
  _textureMap (),
  _imageSize ( imageSize ),
  _parent ( parent ),
  _index ( index ),
  _tileVectorData ( tileVectorData, true ),
  _tileVectorJobs()
{
  USUL_TRACE_SCOPE;

  // We want thread safe ref and unref.
  this->setThreadSafeRefUnref ( true );

  // Set the image.
  if ( true == _image.valid() )
    _texture->setImage ( _image.get() );

#if USE_MIP_MAPS == 1

  // Set filter parameters.
  _texture->setFilter ( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
  _texture->setFilter ( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );

  _texture->setMaxAnisotropy ( ( 0x0 == _body ) ? _texture->getMaxAnisotropy() : _body->maxAnisotropy() );
  _texture->setUseHardwareMipMapGeneration ( true );

  // Set texture coordinate wrapping parameters.
  _texture->setWrap ( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
  _texture->setWrap ( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );

#else

  // Set filter parameters.
  _texture->setFilter ( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
  _texture->setFilter ( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
  
  // Set texture coordinate wrapping parameters.
  _texture->setWrap ( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
  _texture->setWrap ( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );

#endif
  
  _texture->setBorderWidth ( 0.0 );
  _texture->setBorderColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 0.0 ) );

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
  _meshSize ( tile._meshSize ),
  _mesh ( MeshPtr ( static_cast<Mesh*> ( 0x0 ) ) ),
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
  _vector ( new osg::Group ),
  _textureMap ( tile._textureMap ),
  _imageSize ( tile._imageSize ),
  _parent ( tile._parent ),
  _index ( tile._index ),
  _tileVectorData ( 0x0, true ),
  _tileVectorJobs()
{
  USUL_TRACE_SCOPE;

  // Remove if you are ready to test copying. Right now, I'm not sure what 
  // it means. This constructor is here to satisfy the META_Node macro.
  USUL_ASSERT ( false );

  // Handle release build too.
  throw std::runtime_error ( "Error 4113256974: Do not call Tile's copy constructor" );
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

  // At the moment this is redundant but safe.
  this->_cancelTileVectorJobs();
  this->_perTileVectorDataDelete();

  // Don't delete!
  _body = 0x0;

  // Reset the boost shared pointer.
  _mesh = MeshPtr ( static_cast < Mesh * > ( 0x0 ) );

  // Done with the mutex.
  delete _mutex; _mutex = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Tile::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Minerva::Interfaces::ITile::IID:
    return static_cast < Minerva::Interfaces::ITile * > ( this );
  case Minerva::Interfaces::IIntersectNotify::IID:
    return static_cast < Minerva::Interfaces::IIntersectNotify * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the mesh.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::updateMesh()
{
  USUL_TRACE_SCOPE;

  // If they are both not dirty then return.
  if ( ( false == this->verticesDirty() ) && ( false == this->texCoordsDirty() ) )
    return;

  // Get needed variables.
  Extents extents ( this->extents() );
  MeshSize size ( this->meshSize() );
  ElevationDataPtr elevation;
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

  // Depth of skirt.  TODO: This function needs to be tweeked.
  const double offset ( Usul::Math::maximum<double> ( ( 3500 - ( this->level() * 150 ) ), ( 10 * std::numeric_limits<double>::epsilon() ) ) );

  // Make a new mesh.
  MeshPtr mesh ( new Mesh ( size[0], size[1], offset, extents ) );

  // Make a new bounding sphere.
  BSphere boundingSphere;

  // Build the mesh.
  osg::ref_ptr<osg::Node> ground ( mesh->buildMesh ( *body, elevation, osg::Vec2 ( texCoords[0], texCoords[1] ), osg::Vec2 ( texCoords[2], texCoords[3] ), boundingSphere ) );

  // Unset these dirty flags.
  this->dirty ( false, Tile::VERTICES, false );
  this->dirty ( false, Tile::TEX_COORDS, false );

  // Set the ground's alpha.
  OsgTools::State::StateSet::setAlpha ( this, body->alpha() );

  osg::ref_ptr<osg::Group> group ( new osg::Group );
  group->addChild ( ground.get() );
  group->addChild ( Usul::Threads::Safe::get ( this->mutex(), _vector.get() ) );

  // This will add the root node of the container.
  {
    Guard guard ( this );
    group->addChild ( this->_perTileVectorDataGet()->buildScene ( Usul::Interfaces::IBuildScene::Options() ) );
  }

  // Add the low lod if there isn't currently one.
  if ( 0 == this->getNumChildren() )
  {
    this->addChild ( group.get() );
  }

  // Replace the low lod.
  else
  {
    this->setChild ( 0, group.get() );
  }

  // Set needed variables.
  {
    Guard guard ( this );
    _mesh = mesh;
    _boundingSphere = boundingSphere;
  }
  
  this->dirtyBound();
}


//////////////////////////////////////////////////////////////////////////////
//
//  Update the per-tile vector data.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::updateTileVectorData()
{
  USUL_TRACE_SCOPE;

  typedef Usul::Interfaces::ITileVectorJob ITileVectorJob;

  // Copy our container of jobs.
  TileVectorJobs jobs ( Usul::Threads::Safe::get ( this->mutex(), _tileVectorJobs ) );

  // These are the ones we'll remove.
  TileVectorJobs removeMe;

  // Need to update.
  bool needToUpdate ( false );

  // Loop through the container of jobs.
  for ( TileVectorJobs::iterator i = jobs.begin(); i != jobs.end(); ++i )
  {
    // Get needed interface.
    ITileVectorJob::QueryPtr job ( *i );
    if ( true == job.valid() )
    {
      // See if the job is done.
      if ( true == job->isVectorJobDone() )
      {
        // We want to remove this job.
        removeMe.push_back ( *i );

        // Add the data to our container.
        ITileVectorJob::Data data;
        job->takeVectorData ( data );
        for ( ITileVectorJob::Data::iterator d = data.begin(); d != data.end(); ++d )
        {
          // The very first time we add new data we have to clear the 
          // existing data that we inherited from the parent.
          if ( true == this->_perTileVectorDataIsInherited() )
          {
            this->_perTileVectorDataClear();
          }

          // Add the new data.
          Usul::Interfaces::IUnknown::RefPtr layer ( *d );
          TileVectorData::RefPtr tileVectorData ( this->_perTileVectorDataGet() );
          Usul::Functions::safeCallV1V2 ( Usul::Adaptors::memberFunction ( tileVectorData.get(), &TileVectorData::add ), layer.get(), true, "1356847360" );
          needToUpdate = true;
        }
      }
    }
    else
    {
      // Needed interface not available so purge.
      removeMe.push_back ( *i );
    }
  }

  // Remove these jobs.
  if ( false == removeMe.empty() )
  {
    Guard guard ( this );
    for ( TileVectorJobs::iterator j = removeMe.begin(); j != removeMe.end(); ++j )
    {
      Usul::Interfaces::IUnknown::RefPtr job ( *j );
      Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( &_tileVectorJobs, &TileVectorJobs::remove ), job, "1975135769" );
    }
  }

  // Update the borders.
  this->_updateShowBorders();

  // Update container if we need to.
  if ( true == needToUpdate )
  {
    // Get the body as an unknown pointer.
    // Note: I do not think Usul::Threads::Safe::get is safe in this case 
    // because it returns a pointer and we catch it with a smart-pointer. 
    // The _body member could be deferenced and deleted after assignment 
    // to the returned pointer but before catching it with the smart-pointer.
    Usul::Interfaces::IUnknown::QueryPtr body;
    Usul::Threads::Safe::set ( this->mutex(), _body, body );

    // Need to pass the body because it implements the necessary interfaces 
    // to convert the coordinates from lon-lat-elev to x-y-z.
    this->_perTileVectorDataGet()->updateNotify ( body );
  }
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

  // If we have a job, see if it has finished.
  if ( imageJob.valid() )
  {
    if ( imageJob->isDone() )
    {
      // Capture the success state.
      const bool imageJobSuccess ( imageJob->success() );
      
      // Clear the image job.
      {
        Guard guard ( this );
        _imageJob = 0x0;
        imageJob = 0x0;
      }

      // If the job did not succeed, launch a new request.
      if ( false == imageJobSuccess )
        this->_launchImageRequest();
    }
  }

  // If we don't have a job, see if we need to create one.
  else
  {
    if ( Usul::Bits::has ( flags, Tile::IMAGE ) )
      this->_launchImageRequest();
  }

  // For runtime re-scale, etc... not working but flag never true.
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
    // Not currently using this...
#if 0
    // Check for new elevation data.
    if ( elevationJob.valid() && elevationJob->isDone() )
    {
      Guard ( this );
      _elevationJob = 0x0;
      elevationJob = 0x0;
    }
#endif

    // Get cull visitor.
    osgUtil::CullVisitor *cv ( dynamic_cast < osgUtil::CullVisitor * > ( &nv ) );

    // Make sure the mesh is updated.
    this->updateMesh();

    // Make sure our texture is updated.
    this->updateTexture();

    // Make sure the per-tile vector data is up-to-date.
    this->updateTileVectorData();

    // See if we should draw skirts and borders.  Make sure these are called after update mesh.
    this->_setShowSkirts ( body->useSkirts() );
    this->_updateShowBorders();

    // See what we are allowed to do.
    const bool allowSplit ( body->allowSplit() );
    const bool keepDetail ( body->keepDetail() );

    // Do we have a high lod?
    const bool hasDetail ( this->getNumChildren() == 2 );

    // Is this the default behavoir?
    const bool defaultMode ( true == allowSplit && false == keepDetail );

    // Do we need to check if we need more detail?
    const bool checkDetail ( true == allowSplit && true == keepDetail && false == hasDetail );

    // Check if we can split.  Don't freeze if we are waiting for a job.
    const bool splitIfNeeded ( ( false == tileJob.valid() && this->getNumChildren() == 0 ) || true == defaultMode || true == checkDetail );

    // Check to see if we are culled.
    if ( 0x0 == cv || cv->isCulled ( *this ) )
    {
      // We are culled, so cancel these jobs.
      this->_cancelTileVectorJobs();
      this->_cancelTileJob();

      // Do not clear children if we are suppose to keep detail.
      if ( false == keepDetail && false == body->cacheTiles() )
      {
        // Clear our children.
        this->_clearChildren ( true, true );
      }

      // Make sure the borders are updated.
      this->_updateShowBorders();

      // Do not traverse further.
      return;
    }

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

  // Not a cull visitor...
  else
  {
    if ( this->getNumChildren() > 0 )
    {
      this->getChild ( this->getNumChildren() - 1 )->accept ( nv );
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

  // Get needed variables.
  MeshPtr mesh_;
  Usul::Jobs::Job::RefPtr tileJob ( 0x0 );
  Body::RefPtr body ( 0x0 );
  double splitDistance ( 0 );
  {
    Guard guard ( this );
    mesh_ = _mesh;
    tileJob = _tileJob;
    body = _body;
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

  // Do not bother traversing tiles that are facing away.
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

  // Check with smallest distance.
  const double dist ( mesh.getSmallestDistanceSquared ( eye ) );
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
    // Remove high level of detail.
    if ( numChildren > 1 && false == body->cacheTiles() )
    {
      // Need to notify vector data so it can re-adjust.
      Minerva::Core::Data::Container::RefPtr vector ( ( true == body.valid() ) ? body->vectorData() : 0x0 );
      if ( vector.valid() )
      {
        IUnknown::QueryPtr me ( this );
        vector->tileRemovedNotify ( IUnknown::QueryPtr ( this->childAt ( 0 ).get() ), me );
        vector->tileRemovedNotify ( IUnknown::QueryPtr ( this->childAt ( 1 ).get() ), me );
        vector->tileRemovedNotify ( IUnknown::QueryPtr ( this->childAt ( 2 ).get() ), me );
        vector->tileRemovedNotify ( IUnknown::QueryPtr ( this->childAt ( 3 ).get() ), me );
      }

      // Clear all the children.
      this->_clearChildren ( false, true );
    }

    // We no longer need to make the high level of detail.
    this->_cancelTileJob();
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
          
          // The low level will be traversed.  Should the low flag be set to false?
          // At the very least, the high level will be traversed in the next frame.
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

        {
          Guard guard ( this->mutex() );
          _tileJob = 0x0;
          tileJob = 0x0;
        }
      }
    }
  }

  // Update the borders.
  this->_updateShowBorders();
  
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
  USUL_TRACE_SCOPE;

  Body::RefPtr body ( Usul::Threads::Safe::get ( this->mutex(), _body ) );
  
  // Handle no body.
  if ( false == body.valid() )
    return;
  
  const double half ( this->splitDistance() * 0.5 );
  const unsigned int level ( this->level() + 1 );
  
  // Extents for child tiles.
  Extents ll, lr, ul, ur;
  Minerva::Core::Algorithms::split ( this->extents(), ll, lr, ul, ur );
  
  // Mesh sizes for child tiles.
  const MeshSize mll ( body->meshSize ( ll ) );
  const MeshSize mlr ( body->meshSize ( lr ) );
  const MeshSize mul ( body->meshSize ( ul ) );
  const MeshSize mur ( body->meshSize ( ur ) );
  
  // Texture coordinates for the child tiles.
  Usul::Math::Vec4d tll, tlr, tul, tur;
  this->_quarterTextureCoordinates ( tll, tlr, tul, tur );

  Tile::RefPtr t0 ( this->_buildTile ( level, ll, mll, tll, half, job, LOWER_LEFT  ) ); // lower left  tile
  Tile::RefPtr t1 ( this->_buildTile ( level, lr, mlr, tlr, half, job, LOWER_RIGHT ) ); // lower right tile
  Tile::RefPtr t2 ( this->_buildTile ( level, ul, mul, tul, half, job, UPPER_LEFT  ) ); // upper left  tile
  Tile::RefPtr t3 ( this->_buildTile ( level, ur, mur, tur, half, job, UPPER_RIGHT ) ); // upper right tile
  
  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();
  
  // Need to notify vector data so it can re-adjust.
  Minerva::Core::Data::Container::RefPtr vector ( body->vectorData() );
  if ( vector.valid() )
  {
    Usul::Interfaces::IUnknown::QueryPtr unknown ( body );

    // Notify that the elevation has changed.
    vector->elevationChangedNotify ( t0->extents(), t0->level(), t0->elevationData(), unknown.get() );
    vector->elevationChangedNotify ( t1->extents(), t1->level(), t1->elevationData(), unknown.get() );
    vector->elevationChangedNotify ( t2->extents(), t2->level(), t2->elevationData(), unknown.get() );
    vector->elevationChangedNotify ( t3->extents(), t3->level(), t3->elevationData(), unknown.get() );

    // Notify new children have been added.
    IUnknown::QueryPtr me ( this );
    vector->tileAddNotify ( IUnknown::QueryPtr ( t0 ), me );
    vector->tileAddNotify ( IUnknown::QueryPtr ( t1 ), me );
    vector->tileAddNotify ( IUnknown::QueryPtr ( t2 ), me );
    vector->tileAddNotify ( IUnknown::QueryPtr ( t3 ), me );
  }
  
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
//  Get the degrees per pixel.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline Usul::Math::Vec2d degreesPerPixel ( const Tile::Extents& extents, const Tile::ImageSize& size )
  {
    USUL_TRACE_SCOPE_STATIC;
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
  inline Tile::Extents expandExtents ( const Tile::Extents& extents, const Usul::Math::Vec2d& amount )
  {
    USUL_TRACE_SCOPE_STATIC;
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
  USUL_TRACE_SCOPE;

  // If our logic is correct, this should be true.
  USUL_ASSERT ( this->referenceCount() >= 1 );

  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();

  // Get this tile's vector data that falls within the extents.
  TileVectorData::RefPtr tvd ( new TileVectorData );
  tvd->add ( this->_perTileVectorDataGet()->getItemsWithinExtents ( extents.minLon(), extents.minLat(), extents.maxLon(), extents.maxLat() ) );

  // Make the tile.
  Body::RefPtr body ( Usul::Threads::Safe::get ( this->mutex(), _body ) );
  ImageSize imageSize ( Usul::Threads::Safe::get ( this->mutex(), _imageSize ) );
  Tile::RefPtr tile ( new Tile ( this, index, level, extents, size, imageSize, splitDistance, body.get(), 0x0, static_cast<Minerva::Interfaces::IElevationData*> ( 0x0 ), tvd.get() ) );

  // Tell the vector data to update now. Otherwise, when the new tile draws it 
  // will still be blank until the first vector job finishes.
  tvd->updateNotify ( Usul::Interfaces::IUnknown::QueryPtr ( body.get() ) );

  // Tell the tile to start building its elevation data.
  tile->buildElevationData ( job );

  // Use a quarter of the parent's elevation for the child.
  if ( false == tile->elevationData().valid() )
  {
    ElevationDataPtr parentElevation ( Usul::Threads::Safe::get ( this->mutex(), _elevation ) );
    if ( parentElevation.valid() )
    {
      tile->elevationData ( Minerva::Core::Algorithms::resampleElevation ( Tile::RefPtr ( this ), extents ) );
    }
  }

  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();

#if USE_TOP_DOWN_BUILD_RASTER == 0
  // Use the specified region of our image.
  tile->textureData ( this->image().get(), region );
#else

  // Build the raster.  Make sure this is done before mesh is built and texture updated.
  tile->buildRaster ( job );

  // Check to see if the tile has a valid image.
  if ( false == tile->image().valid() && 0x0 != this->image() )
  {
    // Use the specified region of our image.
    tile->textureData ( this->image().get(), region );
  }
#endif

  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();

  tile->updateMesh();
  tile->updateTexture();

  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();

  // Now build the per-tile vector data.
  tile->buildPerTileVectorData ( job );

  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();

#if USE_TOP_DOWN_BUILD_RASTER == 0
  // Make sure the image is dirty.
  tile->dirty ( true, Tile::IMAGE, false );
  
  // Add a request to the queue for this tile now.
  tile->_launchImageRequest();
#endif

  return tile;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build texture for this tile.
//
///////////////////////////////////////////////////////////////////////////////

Tile::ImagePtr Tile::_buildRaster ( const Extents &extents, unsigned int width, unsigned int height, unsigned int level, IRasterLayer* raster, Usul::Jobs::Job::RefPtr job )
{
  USUL_TRACE_SCOPE_STATIC;

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
  USUL_TRACE_SCOPE;

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
    const bool isLevelRange ( layer.valid() ? layer->isInLevelRange ( this->level() ) : true );

    // Only use this layer if it's shown and intersects our extents.
    if ( shown && this->extents().intersects ( e ) && true == isLevelRange )
    {
      // Get the image for the layer.
      image = Tile::_buildRaster ( this->extents(), width, height, this->level(), raster.get(), job.get() );

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
      Minerva::Core::Algorithms::Composite::raster ( *result, *image, tCoords, alphas, alpha, 1.0f, job );
    }
  }

  // Always set our image, even if it's null.
  this->textureData ( result.get(), Usul::Math::Vec4d ( 0.0, 1.0, 0.0, 1.0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build per-tile vector data.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::buildPerTileVectorData ( Usul::Jobs::Job::RefPtr job )
{
  USUL_TRACE_SCOPE;

  // Get the body.
  Body::RefPtr body ( Usul::Threads::Safe::get ( this->mutex(), _body ) );
  if ( false == body.valid() )
    return;

  // Get the needed interface.
  typedef Usul::Interfaces::ITileVectorData ITileVectorData;
  Body::VectorGroup::RefPtr vectorData ( body->vectorData() );
  ITileVectorData::QueryPtr perTileVectorData ( vectorData.get() );
  if ( false == perTileVectorData.valid() )
    return;

  // Need the extents.
  Extents e ( this->extents() );

  // Ask for the container of jobs that we later poll.
  TileVectorJobs tileVectorJobs ( perTileVectorData->launchVectorJobs ( 
    e.minLon(), e.minLat(), e.maxLon(), e.maxLat(), this->level(), body->jobManager(), 
    Usul::Interfaces::IUnknown::QueryPtr ( body ) ) );

  // Have we been cancelled?
  if ( ( 0x0 != job ) && ( true == job->canceled() ) )
    job->cancel();

  // Purge any jobs that are null.
  tileVectorJobs.remove_if ( std::bind2nd ( std::equal_to<TileVectorJobs::value_type>(), TileVectorJobs::value_type ( 0x0 ) ) );

  // Save the jobs.
  Usul::Threads::Safe::set ( this->mutex(), tileVectorJobs, _tileVectorJobs );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image from the given RasterLayer.  May return null.
//
///////////////////////////////////////////////////////////////////////////////

Tile::TextureData Tile::texture ( IRasterLayer * layer ) const
{
  USUL_TRACE_SCOPE;
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
  this->_setDirtyAlways ( state, flags, dirtyChildren );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Mark the dirty state, only if we cross this extents.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::dirty ( bool state, unsigned int flags, bool dirtyChildren, const Extents& extents )
{
  USUL_TRACE_SCOPE;
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

  // Cancel jobs.
  {
    Guard guard ( this );
    Helper::removeAndCancelJob ( _body, _imageJob );
    Helper::removeAndCancelJob ( _body, _tileJob );
  }

  // Set the body to null. We have to do this because the tiles 
  // in jobs may live longer than the body.
  {
    Guard guard ( this );
    _body = 0x0;
  }

  // Delete the per-tile vector data and cancel the jobs.
  this->_cancelTileVectorJobs();
  this->_perTileVectorDataDelete();

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
//  Load the image.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_launchImageRequest()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Cancel the one we have, if any.
  Helper::removeAndCancelJob ( _body, _imageJob );
  
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
    _elevationJob = new Minerva::Core::Jobs::BuildElevation ( this );
    _body->jobManager()->addJob ( _elevationJob.get() );
  }
#endif
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
  Guard guard ( this );

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
  case NONE: break; // If the index is none, fall through to the last return statement.
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
  
  // Set texutre coordinates and image atomiclly.
  {
    Guard guard ( this );
    
    // Set the texture coordinates.
    this->_setTexCoords ( coords );
    
    // Set our image.
    this->_setImage ( image );
  }
  
  // Set children data.
  {
    Usul::Math::Vec4d ll, lr, ul, ur;
    this->_quarterTextureCoordinates ( ll, lr, ul, ur );
    
    // If our children is using the same image that we are, give them the new image with a quarter of the texutre coordinates.
    Guard guard ( this );
    osg::ref_ptr<osg::Image> myImage ( _image );
    if ( _children[LOWER_LEFT].valid()  && myImage.get() == _children[LOWER_LEFT]->image().get()  ) _children[LOWER_LEFT]->textureData  ( image, ll );
    if ( _children[LOWER_RIGHT].valid() && myImage.get() == _children[LOWER_RIGHT]->image().get() ) _children[LOWER_RIGHT]->textureData ( image, lr );
    if ( _children[UPPER_LEFT].valid()  && myImage.get() == _children[UPPER_LEFT]->image().get()  ) _children[UPPER_LEFT]->textureData  ( image, ul );
    if ( _children[UPPER_RIGHT].valid() && myImage.get() == _children[UPPER_RIGHT]->image().get() ) _children[UPPER_RIGHT]->textureData ( image, ur );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the image.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_setImage ( osg::Image* image )
{
  USUL_TRACE_SCOPE;
  _image = image;
  
  // Our image is no longer dirty.
  this->dirty ( false, Tile::IMAGE, false );
  
  // Our texture needs to be updated.
  this->dirty ( true, Tile::TEXTURE, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the starting texture coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_setTexCoords ( const Usul::Math::Vec4d& t )
{
  USUL_TRACE_SCOPE;
  
  if ( false == t.equal ( _texCoords ) )
  {
    _texCoords = t;
    this->dirty ( true, Tile::TEX_COORDS, false );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

Tile::MeshSize Tile::meshSize() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _meshSize;
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
  if ( true == cancelJob )
  {
    // Without this call to cancel we always have to wait for the job to 
    // finish, even if we've already chosen to go down the low path, which 
    // happens when we are zoomed in and then "view all".
    Helper::removeAndCancelJob ( _body, _tileJob );
    
    // Clear the per-tile vector data jobs.
    this->_cancelTileVectorJobs();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear all the jobs.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_cancelTileJob()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  Helper::removeAndCancelJob ( _body, _tileJob );
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

Tile::ElevationDataPtr Tile::elevationData() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _elevation;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation data.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::elevationData ( ElevationDataPtr data )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _elevation = data;
  this->dirty ( true, Tile::VERTICES, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the overall alpha.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::updateAlpha()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  if ( 0x0 != _body )
  {
    OsgTools::State::StateSet::setAlpha ( this, _body->alpha() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add vector data.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::addVectorData ( osg::Node* node )
{
  USUL_TRACE_SCOPE;
  
  if ( 0x0 != node )
  {
    Guard guard ( this );
    _vector->addChild ( node );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove vector data.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::removeVectorData ( osg::Node* node )
{
  USUL_TRACE_SCOPE;
  
  if ( 0x0 != node )
  {
    Guard guard ( this );
    _vector->removeChild ( node );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convience function that re-directs to the body.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3d& point ) const
{
  USUL_TRACE_SCOPE;

  Body* body ( Usul::Threads::Safe::get ( this->mutex(), _body ) );

  if ( 0x0 != body )
  {
    body->latLonHeightToXYZ ( lat, lon, elevation, point );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the job manager.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Jobs::Manager *Tile::jobManager()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( ( 0x0 == _body ) ? 0x0 : _body->jobManager() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the border state.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_setShowBorders ( bool show )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 != _mesh.get() )
  {
    _mesh->showBorder ( show );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the border state.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_updateShowBorders()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  const bool hasTileVectorJobs ( false == _tileVectorJobs.empty() );
  const bool hasTileJob ( true == _tileJob.valid() );
  const bool hasImageJob ( true == _imageJob.valid() );
  const bool isBusy ( hasTileVectorJobs || hasTileJob || hasImageJob );
  const bool allowedToShow ( ( 0x0 == _body ) ? true : _body->useBorders() );

  this->_setShowBorders ( allowedToShow && isBusy );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show skirt state.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_setShowSkirts ( bool show )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( 0x0 != _mesh.get() )
  {
    _mesh->showSkirts ( show );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the per-tile vector data. Create it if needed.
//
///////////////////////////////////////////////////////////////////////////////

Tile::TileVectorData::RefPtr Tile::_perTileVectorDataGet()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  if ( false == _tileVectorData.first.valid() )
  {
    _tileVectorData = TileVectorDataPair ( TileVectorData::RefPtr ( new TileVectorData ), false );
  }
  return _tileVectorData.first;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the per-tile vector data.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_perTileVectorDataClear()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  if ( true == _tileVectorData.first.valid() )
  {
    _tileVectorData.first->clear();
  }
  _tileVectorData.second = false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the per-tile vector data.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_perTileVectorDataDelete()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  if ( true == _tileVectorData.first.valid() )
  {
    _tileVectorData.first = TileVectorData::RefPtr ( 0x0 );
  }
  _tileVectorData.second = false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the data is inherited.
//
///////////////////////////////////////////////////////////////////////////////

bool Tile::_perTileVectorDataIsInherited() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( true == _tileVectorData.second );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel the per-tile vector data jobs.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_cancelTileVectorJobs()
{
  USUL_TRACE_SCOPE;

  typedef Usul::Interfaces::ITileVectorJob ITileVectorJob;

  // Get the jobs and clear our member at the same time.
  TileVectorJobs jobs;
  {
    Guard guard ( this );
    jobs.swap ( _tileVectorJobs );
  }

  // Loop through jobs.
  for ( TileVectorJobs::iterator i = jobs.begin(); i != jobs.end(); ++i )
  {
    // Get needed interface.
    ITileVectorJob::QueryPtr job ( *i );
    if ( true == job.valid() )
    {
      job->cancelVectorJob();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation at lat, lon.
//
///////////////////////////////////////////////////////////////////////////////

double Tile::elevation ( double lat, double lon )
{
  USUL_TRACE_SCOPE;

  // Get the body and mesh.
  Body::RefPtr body;
  MeshPtr mesh;
  {
    Guard guard ( this );
    body = _body;
    mesh = _mesh;
  }

  LandModel::RefPtr land ( body.valid() ? body->landModel() : 0x0 );

  // Return zero if no land model, or no mesh.
  if ( false == land.valid() ||  0x0 == mesh.get())
    return 0.0;

  return mesh->elevation ( lat, lon, *land );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get this tile.
//
//  Note: do not need to guard here because we're returning the pointer to 
//  this object. The only thing to watch out for is this tile getting deleted 
//  in another thread while this function is being executed. However, the 
//  likely scenario is the caller already has an IUnknown::RefPtr to this 
//  object when they query for ITile, so this object should not getd deleted.
//
///////////////////////////////////////////////////////////////////////////////

Tile *Tile::tile()
{
  USUL_TRACE_SCOPE;
  return this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  IUnknown glue.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::ref()
{
  USUL_TRACE_SCOPE;
  BaseClass::ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  IUnknown glue.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::unref ( bool allowDeletion )
{
  USUL_TRACE_SCOPE;
  if ( true == allowDeletion )
  {
    BaseClass::unref();
  }
  else
  {
    BaseClass::unref_nodelete();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the elevation data for this tile.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::buildElevationData ( Usul::Jobs::Job::RefPtr job )
{
  // Have we been cancelled?
  if ( job.valid() && true == job->canceled() )
    job->cancel();

  Body::RefPtr body ( Usul::Threads::Safe::get ( this->mutex(), _body ) );
  const MeshSize size ( Usul::Threads::Safe::get ( this->mutex(), _meshSize ) );
  const Extents extents ( this->extents() );

  // Handle no body.
  if ( false == body.valid() )
    return;
  
  // Expand the request by degreesPerPixel / 2.0 on each side.
  const Extents request ( Helper::expandExtents ( extents, Helper::degreesPerPixel ( extents, size ) ) );

  // Elevation data.
  Minerva::Core::Layers::RasterLayer::RefPtr elevationData ( body->elevationData() );

  // Build the data for elevation.
  if ( 0x0 != elevationData )
  {
    Minerva::Interfaces::IElevationData::RefPtr elevation ( elevationData->elevationData ( 
      request.minLon(), request.minLat(), request.maxLon(), request.maxLat(), size[0], size[1], this->level(), job, 0x0 ) );

    this->elevationData ( elevation );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call to notify of an intersection.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::intersectNotify ( double x, double y, double z, double lon, double lat, double elev, 
                             IUnknown::RefPtr tile, IUnknown::RefPtr body, IUnknown::RefPtr caller, Closest &answer )
{
  USUL_TRACE_SCOPE;
  typedef Minerva::Interfaces::IIntersectNotify IIntersectNotify;
  typedef IIntersectNotify::Path Path;
  typedef IIntersectNotify::Point Point;
  typedef IIntersectNotify::PointAndDistance PointAndDistance;

  IIntersectNotify::QueryPtr notify ( this->_perTileVectorDataGet().get() );
  if ( true == notify.valid() )
  {
    Closest closest ( Path(), PointAndDistance ( Point(), std::numeric_limits<double>::max() ) );
    notify->intersectNotify ( x, y, z, lon, lat, elev, tile, body, caller, closest );

    if ( ( closest.second.second < answer.second.second ) && ( false == closest.first.empty() ) )
    {
      answer.first.insert ( answer.first.end(), Usul::Interfaces::IUnknown::QueryPtr ( this ) );
      answer.first.insert ( answer.first.end(), closest.first.begin(), closest.first.end() );
      answer.second = closest.second;
    }
  }
}
