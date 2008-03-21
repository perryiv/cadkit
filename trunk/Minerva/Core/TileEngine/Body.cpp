
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
//  Body class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/TileEngine/Body.h"
#include "Minerva/Core/Utilities/DirtyTiles.h"
#include "Minerva/Core/Visitor.h"
#include "Minerva/Core/Utilities/Atmosphere.h"

#include "OsgTools/Convert/MatrixTransform.h"
#include "OsgTools/Group.h"
#include "OsgTools/Visitor.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Functions/Execute.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/ILayerExtents.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Threads/Named.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "osg/MatrixTransform"

#include <limits>
#include <stdexcept>

using namespace Minerva::Core::TileEngine;

USUL_FACTORY_REGISTER_CREATOR ( Body );
MINERVA_IMPLEMENT_NODE_CLASS ( Body );


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Body, Body::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Serialization glue for osg::ref_ptr<osg::MatrixTransform>
//
///////////////////////////////////////////////////////////////////////////////

SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( osg::ref_ptr<osg::MatrixTransform> );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Body::Body ( LandModel *land, Usul::Jobs::Manager *manager, const MeshSize &ms, double splitDistance ) : BaseClass(),
  _transform ( new osg::MatrixTransform ),
  _landModel ( land ),
  _rasters ( new RasterGroup ),
  _elevation ( new ElevationGroup ),
  _vectorData ( new VectorGroup ),
  _manager ( manager ),
  _maxLevel ( 50 ),
  _cacheTiles ( false ),
  _splitDistance ( splitDistance ),
  _meshSize ( ms ),
  _useSkirts ( true ),
  _splitCallback ( new Minerva::Core::TileEngine::Callbacks::PassThrough ),
  _scale ( 1 ),
  _deleteTiles(),
  _topTiles(),
  _updateListeners(),
  _allowSplitting ( true ),
  _sky ( new Minerva::Core::Utilities::Atmosphere ),
  _newTexturesLastFrame ( 0 ),
  _needsRedraw ( false )
{
  USUL_TRACE_SCOPE;

  // Serialization setup.
  this->_addMember ( "transformation", _transform );
  this->_addMember ( "land_model", _landModel );
  this->_addMember ( "vector_group", _vectorData );
  this->_addMember ( "raster_group", _rasters );
  this->_addMember ( "elevation_group", _elevation );
  this->_addMember ( "max_level", _maxLevel );
  this->_addMember ( "cache_tiles", _cacheTiles );
  this->_addMember ( "split_distance", _splitDistance );
  this->_addMember ( "mesh_size", _meshSize );
  this->_addMember ( "use_skirts", _useSkirts );
  this->_addMember ( "split_callback", _splitCallback );
  this->_addMember ( "scale", _scale );
  
  // Set the names.
  _elevation->name ( "Elevation" );
  _rasters->name ( "Rasters" );
  _vectorData->name ( "Vector" );
  
  // Add the vector data to the transform.
  _transform->addChild ( _vectorData->buildScene () );
#if 0
  // Set parameters.
  _sky->innerRadius ( land->size() );
  _sky->outerRadius ( _sky->innerRadius() + 400000 );
  
  // Add the sky to the scene.
  _transform->addChild ( _sky );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Body::~Body()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Body::_destroy ), "3973302267" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Body::_destroy()
{
  USUL_TRACE_SCOPE;

  _transform = 0x0;
  _rasters = 0x0;
  _elevation = 0x0;

  // Clear all the tiles.
  Usul::Functions::executeMemberFunctions ( _topTiles, &Tile::clear, true );

  // Done with the list of top-level tiles.
  _topTiles.clear();

  // Purge all tiles.
  this->purgeTiles();

  // Should be ok now.
  _deleteTiles.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Body::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener * > ( this );
  case Usul::Interfaces::IPlanetCoordinates::IID:
    return static_cast < Usul::Interfaces::IPlanetCoordinates* > ( this );
  case Usul::Interfaces::IElevationDatabase::IID:
    return static_cast < Usul::Interfaces::IElevationDatabase * > ( this );
  case Usul::Interfaces::IFrameStamp::IID:
    return static_cast < Usul::Interfaces::IFrameStamp* > ( this );
  case Usul::Interfaces::ITreeNode::IID:
    return static_cast < Usul::Interfaces::ITreeNode* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a tile.
//
///////////////////////////////////////////////////////////////////////////////

void Body::addTile ( const Extents &extents )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Parameters for the tile.
  const unsigned int level  ( 0 );
  const unsigned int width  ( 512 );
  const unsigned int height ( 512 );
  
  // Make the tile.
  const Usul::Math::Vec4d textureCoords ( 0.0, 1.0, 0.0, 1.0 );
  const MeshSize meshSize ( this->meshSize ( extents ) );
  const Tile::ImageSize imageSize ( width, height );
  Tile::RefPtr tile ( new Tile ( 0x0, Tile::NONE, level, extents, meshSize, imageSize, _splitDistance, this ) );

  // Build the raster.
  tile->buildRaster ( 0x0 );

  // Add tile to the transform.
  _transform->addChild ( tile.get() );

  // Add tile to top-level tiles.
  _topTiles.push_back ( tile );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called by the tile when it's no longer needed.
//
///////////////////////////////////////////////////////////////////////////////

void Body::_addTileToBeDeleted ( Tile *tile )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 != tile )
  {
    _deleteTiles.push_back ( Tile::RefPtr ( tile ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Node *Body::scene() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _transform.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *Body::scene()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _transform.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the center.
//
///////////////////////////////////////////////////////////////////////////////

void Body::center ( const Vec3d &c )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _transform->setMatrix ( osg::Matrixd::translate ( c[0], c[1], c[2] ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the center.
//
///////////////////////////////////////////////////////////////////////////////

Body::Vec3d Body::center() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  const osg::Vec3d c ( _transform->getMatrix().getTrans() );
  return Vec3d ( c[0], c[1], c[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append raster data.
//
///////////////////////////////////////////////////////////////////////////////

void Body::rasterAppend ( Usul::Interfaces::IRasterLayer * layer )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( ( true == _rasters.valid() ) && ( 0x0 != layer ) )
  {
    // Append the layer to the existing group.
    _rasters->append ( layer );
    
    // Get the extents.
    Extents e ( this->_buildExtents( layer ) );

    // Dirty the tiles.
    this->dirtyTextures ( e );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove raster layer.
//
///////////////////////////////////////////////////////////////////////////////

void Body::rasterRemove ( Usul::Interfaces::IRasterLayer *layer )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  if ( ( true == _rasters.valid() ) && ( 0x0 != layer ) )
  {
    // Append the layer to the existing group.
    _rasters->remove ( layer );
    
    // Get the extents.
    Extents e ( this->_buildExtents( layer ) );
    
    // Dirty the tiles.
    this->dirtyTextures ( e );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Raster has chagned.
//
///////////////////////////////////////////////////////////////////////////////

void Body::rasterChanged (  Usul::Interfaces::IRasterLayer *layer )
{
  if ( 0x0 != layer )
  {
    // Get the extents.
    Extents e ( this->_buildExtents( layer ) );
    
    // Dirty the tiles.
    this->dirtyTextures ( e );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  dirty textures.
//
///////////////////////////////////////////////////////////////////////////////

void Body::dirtyTextures ( const Extents& e )
{
  // Dirty the tiles.
  Minerva::Core::Utilities::DirtyTiles dirty ( true, Tile::IMAGE /*, e*/ );
  osg::ref_ptr<osg::NodeVisitor> visitor ( OsgTools::MakeVisitor<osg::Group>::make ( dirty ) );
  _transform->accept ( *visitor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the raster data.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Layers::RasterLayer::RefPtr Body::rasterData()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return RasterLayer::RefPtr ( _rasters.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert lat,lon,height to x,y,z.
//
///////////////////////////////////////////////////////////////////////////////

void Body::latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3d& point ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( true == _landModel.valid() )
  {
    _landModel->latLonHeightToXYZ ( lat, lon, elevation, point );

    // Scale
    point *= _scale;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert x,y,z to lat, lon, height.
//
///////////////////////////////////////////////////////////////////////////////

void Body::xyzToLatLonHeight ( const osg::Vec3d& point, double& lat, double& lon, double& elevation ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( true == _landModel.valid() )
  {
    // Make a copy.
    osg::Vec3d p ( point );

    // Restore scale
    p /= _scale;

    _landModel->xyzToLatLonHeight ( p, lat, lon, elevation );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert to planet coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void Body::convertToPlanet ( const Usul::Math::Vec3d& orginal, Usul::Math::Vec3d& planetPoint ) const
{
  USUL_TRACE_SCOPE;
  osg::Vec3d out;
  this->latLonHeightToXYZ ( orginal[1], orginal[0], orginal[2], out );
  planetPoint.set ( out[0], out[1], out[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert from planet coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void Body::convertFromPlanet ( const Usul::Math::Vec3d& planetPoint, Usul::Math::Vec3d& latLonPoint ) const
{
  USUL_TRACE_SCOPE;
  this->xyzToLatLonHeight ( osg::Vec3d ( planetPoint[0], planetPoint[1], planetPoint[2] ), latLonPoint[1], latLonPoint[0], latLonPoint[2] );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Matrix to place items on the planet (i.e. local coordinates to world coordinates).
//
///////////////////////////////////////////////////////////////////////////////

osg::Matrixd Body::planetRotationMatrix ( double lat, double lon, double elevation, double heading ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  return ( _landModel.valid() ? _landModel->planetRotationMatrix ( lat, lon, elevation, heading ) : osg::Matrixd() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the job manager for this body.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Jobs::Manager *Body::jobManager()
{
  USUL_TRACE_SCOPE;
  JobManager::GuardType guard ( _manager.mutex() );
  return _manager.value();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the job manager.
//
///////////////////////////////////////////////////////////////////////////////

void Body::jobManager ( Usul::Jobs::Manager *manager )
{
  USUL_TRACE_SCOPE;
  JobManager::GuardType guard ( _manager.mutex() );
  _manager = manager;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer is about to render.
//
///////////////////////////////////////////////////////////////////////////////

void Body::preRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  BaseClass::preRender ( caller );
  
  // Reset the number of new textures.
  Guard guard ( this->mutex() );
  _newTexturesLastFrame = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer just rendered.
//
///////////////////////////////////////////////////////////////////////////////

void Body::postRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  // Remove all tiles that are ready.
  this->purgeTiles();

  // Pass along to base class.
  BaseClass::postRender ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Request texture.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Jobs::BuildRaster::RefPtr Body::textureRequest ( Tile* tile )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 == this->jobManager() )
  {
    // Have to throw because there's no value we can return.
    throw std::runtime_error ( "Error 3925869673: Job manager is null" );
  }

  BuildRaster::RefPtr job ( new BuildRaster ( Tile::RefPtr ( tile ) ) );
  this->jobManager()->addJob ( job );

  return job;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the maximum level.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Body::maxLevel() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _maxLevel;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the maximum level.
//
///////////////////////////////////////////////////////////////////////////////

void Body::maxLevel ( unsigned int level )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _maxLevel = level;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flag for caching tiles.
//
///////////////////////////////////////////////////////////////////////////////

bool Body::cacheTiles() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _cacheTiles;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag for caching tiles.
//
///////////////////////////////////////////////////////////////////////////////

void Body::cacheTiles ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _cacheTiles = state;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mesh size for the extents.
//
///////////////////////////////////////////////////////////////////////////////

Body::MeshSize Body::meshSize ( const Body::Extents &extents )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( ( true == _landModel.valid() ) ? _landModel->meshSize ( extents, _meshSize ) : _meshSize );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag to use skirts.
//
///////////////////////////////////////////////////////////////////////////////

void Body::useSkirts ( bool use )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Set the flag.
  _useSkirts = use;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flag to use skirts.
//
///////////////////////////////////////////////////////////////////////////////

bool Body::useSkirts() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _useSkirts;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append elevation data.
//
///////////////////////////////////////////////////////////////////////////////

void Body::elevationAppend ( Usul::Interfaces::IRasterLayer * layer )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( ( true == _elevation.valid() ) && ( 0x0 != layer ) )
  {
    // Append the layer to the existing group.
    _elevation->append ( layer );
    
    // Get the extents.
    Extents e ( this->_buildExtents( layer ) );

    // Dirty the tiles.
    Minerva::Core::Utilities::DirtyTiles dirty ( true, Tile::VERTICES, e );
    osg::ref_ptr<osg::NodeVisitor> visitor ( OsgTools::MakeVisitor<osg::Group>::make ( dirty ) );
    _transform->accept ( *visitor );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation data.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Layers::RasterLayer::RefPtr Body::elevationData()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return RasterLayer::RefPtr ( _elevation.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the tile should split.
//
///////////////////////////////////////////////////////////////////////////////

bool Body::shouldSplit ( bool suggestion, Tile *tile )
{
  USUL_TRACE_SCOPE;
  SplitCallback::RefPtr callback ( this->splitCallback() );
  return ( ( true == callback.valid() ) ? callback->shouldSplit ( suggestion, tile ) : suggestion );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the split callback..
//
///////////////////////////////////////////////////////////////////////////////

void Body::splitCallback ( SplitCallback *cb )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _splitCallback = cb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the split callback..
//
///////////////////////////////////////////////////////////////////////////////

Body::SplitCallback::RefPtr Body::splitCallback() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return SplitCallback::RefPtr ( _splitCallback );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scale.
//
///////////////////////////////////////////////////////////////////////////////

void Body::scale( double s )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _scale = s;

  // Dirty the tiles.
  Minerva::Core::Utilities::DirtyTiles dirty ( true, Tile::VERTICES | Tile::CHILDREN, Extents( -180, -90, 180, 90 ) );
  osg::ref_ptr<osg::NodeVisitor> visitor ( OsgTools::MakeVisitor<osg::Group>::make ( dirty ) );
  _transform->accept ( *visitor );

  _transform->dirtyBound();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scale.
//
///////////////////////////////////////////////////////////////////////////////

double Body::scale() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _scale;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Serialize the members.
//
///////////////////////////////////////////////////////////////////////////////

void Body::serialize ( XmlTree::Node &parent ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Copy the map.
  Serialize::XML::DataMemberMap m ( _dataMemberMap );

  // We serialize a vector of extents.
  typedef std::vector < Usul::Math::Vec4d > Tiles;
  Tiles tiles;

  // Loop through tiles.
  for ( unsigned int i = 0; i < _transform->getNumChildren(); ++i )
  {
    // Get the tile.
    Tile::RefPtr tile ( dynamic_cast < Tile * > ( _transform->getChild ( i ) ) );
    if ( true == tile.valid() )
    {
      // Add the extents.
      const Tile::Extents e ( tile->extents() );
      tiles.push_back ( Usul::Math::Vec4d ( e.minimum()[0], e.minimum()[1], e.maximum()[0], e.maximum()[1] ) );
    }
  }

  // Add additional entries.
  m.addMember ( "tiles", tiles );

  // Write members to the node from local map.
  m.serialize ( parent );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize the members.
//
///////////////////////////////////////////////////////////////////////////////

void Body::deserialize ( const XmlTree::Node &node )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Copy the map.
  Serialize::XML::DataMemberMap m ( _dataMemberMap );

  // We serialize a vector of extents.
  typedef std::vector < Usul::Math::Vec4d > Tiles;
  Tiles tiles;

  // Add additional entries.
  m.addMember ( "tiles", tiles );

  // Initialize locals and members from the the node.
  m.deserialize ( node );

  // Loop through tiles.
  for ( Tiles::const_iterator i = tiles.begin(); i != tiles.end(); ++i )
  {
    // Add the tile.
    const Tiles::value_type e ( *i );
    this->addTile ( Tile::Extents ( e[0], e[1], e[2], e[3] ) );
  }
  
  // Add the vector data to the transform.
  _transform->addChild ( _vectorData->buildScene () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the split distance.
//
///////////////////////////////////////////////////////////////////////////////

void Body::splitDistance ( double d, bool children )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _splitDistance = d;

  for ( Tiles::iterator iter = _topTiles.begin(); iter != _topTiles.end(); ++iter )
  {
    // Set new split distance.
    Tiles::value_type tile ( *iter );
    tile->splitDistance ( d, children );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the split distance.
//
///////////////////////////////////////////////////////////////////////////////

double Body::splitDistance() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _splitDistance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the extents of the unknown.
//
///////////////////////////////////////////////////////////////////////////////

Body::Extents Body::_buildExtents ( Usul::Interfaces::IUnknown* unknown )
{
  Usul::Interfaces::ILayerExtents::QueryPtr le ( unknown );
  
  const double minLon ( le.valid() ? le->minLon() : -180.0 );
  const double minLat ( le.valid() ? le->minLat() :  -90.0 );
  const double maxLon ( le.valid() ? le->maxLon() :  180.0 );
  const double maxLat ( le.valid() ? le->maxLat() :   90.0 );
  
  return Extents ( minLon, minLat, maxLon, maxLat );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation at a lat, lon.
//
///////////////////////////////////////////////////////////////////////////////

double Body::elevation ( double lat, double lon ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  RasterGroup::RefPtr elevation ( Usul::Threads::Safe::get ( this->mutex(), _elevation.get() ) );

  // Get the number of layers.
  const int number ( elevation->size() );

  // Loop through the elevation data sets from last to first.
  for ( int i = number - 1; i >= 0; --i )
  {
    Usul::Interfaces::IElevationDatabase::QueryPtr data ( elevation->layer ( i ) );
    if ( data.valid() )
    {
      // Get the height for this layer.
      const double h ( data->elevationAtLatLong ( lat, lon ) );
      if ( h > 0.0 )
        return h;
    }
  }
  
  // Should we use a geoid here?  Keeping this for reference.
#if 0
  ossimGpt point ( lat, lon );
  double height (  ossimElevManager::instance()->getHeightAboveMSL( point ) );
  if( ossim::isnan ( height ) )
    height = 0.0;
  
  return height + ossimGeoidManager::instance()->offsetFromEllipsoid( point );
#endif

  return 0.0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the rasters.
//
///////////////////////////////////////////////////////////////////////////////

void Body::rasters ( Rasters& rasters ) const
{
  USUL_TRACE_SCOPE;

  RasterGroup::RefPtr group ( Usul::Threads::Safe::get ( this->mutex(), _rasters ) );
  
  if ( true == group.valid() )
  {
    group->layers ( rasters );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Purge all tiles that are ready.
//
///////////////////////////////////////////////////////////////////////////////

void Body::purgeTiles()
{
  USUL_TRACE_SCOPE;

  // Swap with the list to delete.
  Tiles deleteMe;
  {
    Guard guard ( this );
    if ( false == _deleteTiles.empty() )
    {
      _deleteTiles.swap ( deleteMe );
    }
  }

  // Clear the list of tiles to be deleted.
  if ( false == deleteMe.empty() )
  {
    Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( &deleteMe, &Tiles::clear ), "4101333810" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the update listener.
//
///////////////////////////////////////////////////////////////////////////////

void Body::_addUpdateListener ( IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  
  // Don't add twice.
  this->_removeUpdateListener ( caller );
  
  IUpdateListener::QueryPtr listener ( caller );
  if ( true == listener.valid() )
  {
    Guard guard ( this->mutex() );
    _updateListeners.push_back ( IUpdateListener::RefPtr ( listener.get() ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void Body::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  // Update.
  Usul::Interfaces::IUnknown::QueryPtr unknown ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  
  // Update the vector group.
  _vectorData->updateNotify ( unknown );
  
  // Update any other listeners.
  std::for_each ( _updateListeners.begin(), _updateListeners.end(), std::bind2nd ( std::mem_fun ( &IUpdateListener::updateNotify ), unknown.get() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the listener.
//
///////////////////////////////////////////////////////////////////////////////

void Body::_removeUpdateListener ( IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  
  IUpdateListener::QueryPtr listener ( caller );
  if ( true == listener.valid() )
  {
    Guard guard ( this->mutex() );
    IUpdateListener::RefPtr value ( listener.get() );
    UpdateListeners::iterator end ( std::remove ( _updateListeners.begin(), _updateListeners.end(), value ) );
    _updateListeners.erase ( end, _updateListeners.end() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the frame stamp.
//  
///////////////////////////////////////////////////////////////////////////////

osg::FrameStamp * Body::frameStamp()
{
  USUL_TRACE_SCOPE;
  Usul::Interfaces::IFrameStamp::QueryPtr fs ( Usul::Documents::Manager::instance().activeView() );
  return ( fs.valid() ? fs->frameStamp() : 0x0 );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the frame stamp.
//  
///////////////////////////////////////////////////////////////////////////////

const osg::FrameStamp * Body::frameStamp() const
{
  USUL_TRACE_SCOPE;
  Usul::Interfaces::IFrameStamp::QueryPtr fs ( Usul::Documents::Manager::instance().activeView() );
  return ( fs.valid() ? fs->frameStamp() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Body::getNumChildNodes() const
{
  USUL_TRACE_SCOPE;
  return 3;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child node (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITreeNode * Body::getChildNode ( unsigned int which )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  if ( 0 == which )
    return Usul::Interfaces::ITreeNode::QueryPtr ( _elevation.get() );
  else if ( 1 == which )
    return Usul::Interfaces::ITreeNode::QueryPtr ( _rasters.get() );
  
  return Usul::Interfaces::ITreeNode::QueryPtr ( _vectorData.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

void Body::setTreeNodeName ( const std::string & )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

std::string Body::getTreeNodeName() const
{
  USUL_TRACE_SCOPE;
  return "Body";
}



///////////////////////////////////////////////////////////////////////////////
//
//  Add vector data.
//
///////////////////////////////////////////////////////////////////////////////

void Body::vectorAppend ( Usul::Interfaces::IUnknown *unknown )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  // Add the layer to our group.
  if ( Minerva::Core::Layers::Vector *vector = dynamic_cast< Minerva::Core::Layers::Vector*> ( unknown ) )
  {
    _vectorData->addLayer ( vector );
    
    // Add to the update listeners.
    this->_addUpdateListener( unknown );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vector data.
//
///////////////////////////////////////////////////////////////////////////////

Body::VectorGroup::RefPtr Body::vectorData()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _vectorData.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove vector data.
//
///////////////////////////////////////////////////////////////////////////////

void Body::vectorRemove ( Usul::Interfaces::IUnknown *unknown )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  // Add the layer to our group.
  if ( Minerva::Core::Layers::Vector *vector = dynamic_cast< Minerva::Core::Layers::Vector*> ( unknown ) )
  {
    _vectorData->removeLayer ( vector );
    
    // Add to the update listeners.
    this->_removeUpdateListener( unknown );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation at a lat, lon (IElevationDatabase).
//
///////////////////////////////////////////////////////////////////////////////

double Body::elevationAtLatLong ( double lat, double lon ) const
{
  USUL_TRACE_SCOPE;
  return this->elevation ( lat, lon );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag that says to allow spliting.
//
///////////////////////////////////////////////////////////////////////////////

void Body::allowSplitting ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _allowSplitting = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flag that says to allow spliting.
//
///////////////////////////////////////////////////////////////////////////////

bool Body::allowSplitting() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _allowSplitting;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of new textures last frame.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Body::newTexturesLastFrame() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _newTexturesLastFrame;
}


///////////////////////////////////////////////////////////////////////////////
//
//  A texture has been added.
//
///////////////////////////////////////////////////////////////////////////////

void Body::_textureAdded()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  ++_newTexturesLastFrame;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the needs redraw state.
//
///////////////////////////////////////////////////////////////////////////////

void Body::needsRedraw ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _needsRedraw = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the needs redraw state.
//
///////////////////////////////////////////////////////////////////////////////

bool Body::needsRedraw() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _needsRedraw;
}
