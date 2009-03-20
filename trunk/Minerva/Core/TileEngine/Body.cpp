
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
#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/Line.h"
#include "Minerva/Core/Utilities/Atmosphere.h"
#include "Minerva/Core/Visitor.h"

#include "OsgTools/Convert.h"
#include "OsgTools/Convert/MatrixTransform.h"
#include "OsgTools/Group.h"
#include "OsgTools/Ray.h"
#include "OsgTools/Visitor.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Functions/Execute.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/ILayerExtents.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Registry/Database.h"
#include "Usul/Threads/Named.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "osg/MatrixTransform"

#include "osgUtil/LineSegmentIntersector"

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
//  The number for the renderbin used by the vector data.
//
///////////////////////////////////////////////////////////////////////////////

const int VECTOR_RENDER_BIN_NUMBER ( 10000 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Body::Body ( LandModel *land, Usul::Jobs::Manager *manager, const MeshSize &ms, double splitDistance ) : BaseClass(),
  _transform ( new osg::MatrixTransform ),
  _graphic ( new VectorGroup ),
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
  _useBorders ( true ),
  _splitCallback ( 0x0 ),
  _scale ( 1 ),
  _deleteTiles(),
  _topTiles(),
  _updateListeners(),
  _allowSplit ( true ),
  _keepDetail ( false ),
  _sky ( 0x0 ),
  _newTexturesLastFrame ( 0 ),
  _needsRedraw ( false ),
  _log ( 0x0 ),
  _name( "Body" ),
  _imageSize ( 256, 256 ),
  _alpha ( 1.0f ),
  _maxAnisotropy ( Usul::Registry::Database::instance()["default_max_anisotropy"].get<float> ( 16.0f, true ) ),
  SERIALIZE_XML_INITIALIZER_LIST
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
  this->_addMember ( "use_borders", _useBorders );
  this->_addMember ( "split_callback", _splitCallback );
  this->_addMember ( "scale", _scale );
  this->_addMember ( "name", _name );
  this->_addMember ( "image_size", _imageSize );
  this->_addMember ( "alpha", _alpha );
  this->_addMember ( "max_anisotropic_texture_filtering", _maxAnisotropy );

  // Set the names.
  _elevation->name ( "Elevation" );
  _rasters->name ( "Rasters" );
  _vectorData->name ( "Vector" );
  _graphic->name ( "Intersection Graphic" );
  
  // Add the vector data to the transform. Make sure it is drawn last.
  osg::ref_ptr<osg::Node> vectorData ( _vectorData->buildScene ( Usul::Interfaces::IBuildScene::Options() ) );
  vectorData->getOrCreateStateSet()->setRenderBinDetails ( VECTOR_RENDER_BIN_NUMBER, "RenderBin" );
  _transform->addChild ( vectorData.get() );

  // Add the branch for intersection graphics. Make sure it is drawn last.
  osg::ref_ptr<osg::Node> graphic ( _graphic->buildScene ( Usul::Interfaces::IBuildScene::Options() ) );
  graphic->getOrCreateStateSet()->setRenderBinDetails ( VECTOR_RENDER_BIN_NUMBER, "RenderBin" );
  _transform->addChild ( graphic.get() );

#if 0
  // Make the sky.
  _sky = new Minerva::Core::Utilities::Atmosphere;
  
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
  this->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to safely clear and set to null.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class SmartPointerType > void safeClear ( SmartPointerType ptr )
  {
    if ( true == ptr.valid() )
    {
      ptr->clear();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the body.
//
///////////////////////////////////////////////////////////////////////////////

void Body::clear()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _transform = 0x0;
  _landModel = 0x0;

  Helper::safeClear ( _rasters    ); _rasters    = 0x0;
  Helper::safeClear ( _elevation  ); _elevation  = 0x0;
  Helper::safeClear ( _vectorData ); _vectorData = 0x0;
  Helper::safeClear ( _graphic    ); _graphic    = 0x0;

  _splitCallback = 0x0;

  Usul::Functions::executeMemberFunctions ( _topTiles,    &Tile::clear, true ); _topTiles.clear();
  Usul::Functions::executeMemberFunctions ( _deleteTiles, &Tile::clear, true ); _deleteTiles.clear();

  _updateListeners.clear();

  _sky = 0x0;
  _log = 0x0;

  _name.clear();
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
  case Usul::Interfaces::ITreeNode::IID:
    return static_cast < Usul::Interfaces::ITreeNode* > ( this );
  case Usul::Interfaces::IRasterAlphas::IID:
    return static_cast < Usul::Interfaces::IRasterAlphas * > ( this );
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
  
  // Make the tile.
  const Usul::Math::Vec4d textureCoords ( 0.0, 1.0, 0.0, 1.0 );
  const MeshSize meshSize ( this->meshSize ( extents ) );
  Tile::RefPtr tile ( new Tile ( 0x0, Tile::NONE, level, extents, meshSize, _imageSize, _splitDistance, this ) );

  // Build the raster.
  // If a network layer times out and throws an exception, the document won't load.
  // Wrap in a safe call to ensure the document still loads (but the top level tiles won't have textures).
  Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( tile.get(), &Tile::buildRaster ), Usul::Jobs::Job::RefPtr ( 0x0 ), "4827869570" );

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

void Body::_addTileToBeDeleted ( Tile::RefPtr tile )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( true == tile.valid() )
  {
    _deleteTiles.push_back ( tile );
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
  this->_dirtyTiles ( Tile::IMAGE, e );
}


///////////////////////////////////////////////////////////////////////////////
//
//  dirty vertices.
//
///////////////////////////////////////////////////////////////////////////////

void Body::dirtyVertices()
{
  this->_dirtyTiles ( Tile::VERTICES );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dirty the tiles.
//
///////////////////////////////////////////////////////////////////////////////

void Body::_dirtyTiles ( unsigned int flags )
{
  USUL_TRACE_SCOPE;
  
  Tiles tiles ( Usul::Threads::Safe::get ( this->mutex(), _topTiles ) );
  for ( Tiles::iterator iter = tiles.begin(); iter != tiles.end(); ++iter )
  {
    Tile::RefPtr tile ( *iter );
    if ( tile.valid() )
      tile->dirty ( true, flags, true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dirty the tiles within extents.
//
///////////////////////////////////////////////////////////////////////////////

void Body::_dirtyTiles ( unsigned int flags, const Extents& extents )
{
  USUL_TRACE_SCOPE;
  
  Tiles tiles ( Usul::Threads::Safe::get ( this->mutex(), _topTiles ) );
  for ( Tiles::iterator iter = tiles.begin(); iter != tiles.end(); ++iter )
  {
    Tile::RefPtr tile ( *iter );
    if ( tile.valid() )
      tile->dirty ( true, flags, true, extents );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the mesh's alpha.
//
///////////////////////////////////////////////////////////////////////////////

void Body::updateTilesAlpha()
{
  osg::ref_ptr<osg::NodeVisitor> visitor ( OsgTools::MakeVisitor<osg::Group>::make 
    ( Usul::Adaptors::memberFunction ( this, &Body::_updateTileAlpha ) ) );
  _transform->accept ( *visitor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the tile's alpha.
//
///////////////////////////////////////////////////////////////////////////////

void Body::_updateTileAlpha ( osg::Group *group )
{
  Tile::RefPtr tile ( dynamic_cast < Tile * > ( group ) );
  if ( true == tile.valid() )
  {
    tile->updateAlpha();
  }
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
    _landModel->latLonHeightToXYZ ( lat, lon, elevation, point.x(), point.y(), point.z() );

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

    _landModel->xyzToLatLonHeight ( p.x(), p.y(), p.z(), lat, lon, elevation );
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

void Body::convertFromPlanet ( const Usul::Math::Vec3d& planetPoint, Usul::Math::Vec3d& lonLatPoint ) const
{
  USUL_TRACE_SCOPE;
  this->xyzToLatLonHeight ( osg::Vec3d ( planetPoint[0], planetPoint[1], planetPoint[2] ), lonLatPoint[1], lonLatPoint[0], lonLatPoint[2] );
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
  
  typedef LandModel::Matrix Matrix;
  Matrix matrix ( _landModel.valid() ? _landModel->planetRotationMatrix ( lat, lon, elevation, heading ) : Matrix() );

  osg::Matrixd result;
  OsgTools::Convert::matrix ( matrix, result );
  return result;
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
  this->jobManager()->addJob ( job.get() );

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
//  Set the flag to use borders.
//
///////////////////////////////////////////////////////////////////////////////

void Body::useBorders ( bool use )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Set the flag.
  _useBorders = use;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flag to use borders.
//
///////////////////////////////////////////////////////////////////////////////

bool Body::useBorders() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _useBorders;
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

    this->dirtyVertices();
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

Body::SplitCallback::RefPtr Body::splitCallback()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  if ( false == _splitCallback.valid() )
  {
    _splitCallback = new Minerva::Core::TileEngine::Callbacks::PassThrough;
  }
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

  this->_dirtyTiles ( Tile::VERTICES | Tile::CHILDREN, Extents( -180, -90, 180, 90 ) );

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
  
  // Re-add these scenes to the transform because a new one was just created.
  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Make sure the line is drawn last.
  osg::ref_ptr<osg::Node> vectorData ( _vectorData->buildScene ( Usul::Interfaces::IBuildScene::Options() ) );
  vectorData->getOrCreateStateSet()->setRenderBinDetails ( VECTOR_RENDER_BIN_NUMBER, "RenderBin" );
  _transform->addChild ( vectorData.get() );

  // Make sure the line is drawn last.
  osg::ref_ptr<osg::Node> graphic ( _graphic->buildScene ( Usul::Interfaces::IBuildScene::Options() ) );
  graphic->getOrCreateStateSet()->setRenderBinDetails ( VECTOR_RENDER_BIN_NUMBER, "RenderBin" );
  _transform->addChild ( graphic.get() );
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
//  Get the elevation from a tile.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  bool elevationFromTile ( Tile::RefPtr tile, const Body::Extents::Vertex& p, double& elevation )
  {
    if ( false == tile.valid() )
      return false;

    typedef Tile::Extents Extents;

    Extents e ( tile->extents() );

    if ( e.contains ( p ) )
    {
      if ( tile->isLeaf() )
      {
#if 1
        elevation = tile->elevation ( p[1], p[0] );
        return true;
#else
        // Use tile's elevation data.
        osg::ref_ptr<osg::Image> elevationData ( tile->elevation() );

        if ( elevationData.valid() )
        {
          // Size of the elevation grid.
          const unsigned int width  ( elevationData->s() );
          const unsigned int height ( elevationData->t() );

          // Shortcuts.
          const Extents::Vertex &mn ( e.minimum() );
          const Extents::Vertex &mx ( e.maximum() );

          const double u ( ( p[0] - mn[0] ) / ( mx[0] - mn[0] ) );
          const double v ( ( p[1] - mn[1] ) / ( mx[1] - mn[1] ) );

          const unsigned int i ( Usul::Math::minimum ( width - 1,  static_cast <unsigned int> ( ::floor ( ( u * width ) + 0.5 ) ) ) );
          const unsigned int j ( Usul::Math::minimum ( height - 1, static_cast <unsigned int> ( ::floor ( ( v * height ) + 0.5 ) ) ) );

          // Set the elevation data.
          elevation = *reinterpret_cast<float*> ( elevationData->data ( i, j ) );

          return true;

        }
#endif
      }
      else
      {
        // Ask the children.
        if ( Detail::elevationFromTile ( tile->childAt ( 0 ), p, elevation ) ) return true;
        if ( Detail::elevationFromTile ( tile->childAt ( 1 ), p, elevation ) ) return true;
        if ( Detail::elevationFromTile ( tile->childAt ( 2 ), p, elevation ) ) return true;
        if ( Detail::elevationFromTile ( tile->childAt ( 3 ), p, elevation ) ) return true;
      }
    }

    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation at a lat, lon.
//
///////////////////////////////////////////////////////////////////////////////

double Body::elevation ( double lat, double lon ) const
{
  USUL_TRACE_SCOPE;

  Tiles tiles ( Usul::Threads::Safe::get ( this->mutex(), _topTiles ) );
  
  Extents::Vertex v ( lon, lat );
  double elevation ( 0.0 );

  for ( Tiles::const_iterator iter = tiles.begin(); iter != tiles.end(); ++iter )
  {
    if ( Detail::elevationFromTile ( *iter, v, elevation ) ) 
      break;
  }
  
  return elevation;
  
  // Should we use a geoid here?  Keeping this for reference.
  // http://en.wikipedia.org/wiki/Geoid
#if 0
  ossimGpt point ( lat, lon );
  double height (  ossimElevManager::instance()->getHeightAboveMSL( point ) );
  if( ossim::isnan ( height ) )
    height = 0.0;
  
  return height + ossimGeoidManager::instance()->offsetFromEllipsoid( point );
#endif
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
  _updateListeners.add ( caller );
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
  _graphic->updateNotify ( unknown );
  
  // Update any other listeners.
  _updateListeners.for_each ( std::bind2nd ( std::mem_fun ( &IUpdateListener::updateNotify ), unknown.get() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the listener.
//
///////////////////////////////////////////////////////////////////////////////

void Body::_removeUpdateListener ( IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  _updateListeners.remove ( caller );  
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

Usul::Interfaces::ITreeNode::RefPtr Body::getChildNode ( unsigned int which )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  typedef Usul::Interfaces::ITreeNode ITreeNode;

  switch ( which )
  {
    case 0:
      return ITreeNode::RefPtr ( ITreeNode::QueryPtr ( _elevation.get() ) );
    case 1:
      return ITreeNode::RefPtr ( ITreeNode::QueryPtr ( _rasters.get() ) );
    case 2:
      return ITreeNode::RefPtr ( ITreeNode::QueryPtr ( _vectorData.get() ) );
    default:
      return ITreeNode::RefPtr ( 0x0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

void Body::setTreeNodeName ( const std::string & s )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _name = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

std::string Body::getTreeNodeName() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _name;
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
  
  Usul::Interfaces::IBuildScene::QueryPtr build ( unknown );

  if ( build.valid() )
  {
    // Add the layer to our group.
    _vectorData->add ( unknown );
      
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
  _vectorData->remove ( unknown );
    
  // Add to the update listeners.
  this->_removeUpdateListener( unknown );
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

void Body::allowSplit ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _allowSplit = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flag that says to allow spliting.
//
///////////////////////////////////////////////////////////////////////////////

bool Body::allowSplit() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _allowSplit;
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


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag that says to keep detail.
//
///////////////////////////////////////////////////////////////////////////////

void Body::keepDetail ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _keepDetail = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flag that says to keep detail.
//
///////////////////////////////////////////////////////////////////////////////

bool Body::keepDetail() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _keepDetail;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the log.
//
///////////////////////////////////////////////////////////////////////////////

void Body::logSet ( LogPtr lp )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _log = lp;

  if ( true == _rasters.valid() )
    _rasters->logSet ( lp );

  if ( true == _elevation.valid() )
    _elevation->logSet ( lp );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the log.
//
///////////////////////////////////////////////////////////////////////////////

Body::LogPtr Body::logGet()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return LogPtr ( _log );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the land model.
//
///////////////////////////////////////////////////////////////////////////////

LandModel* Body::landModel() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _landModel.get();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the alpha value.
//
///////////////////////////////////////////////////////////////////////////////

void Body::alpha ( float a )
{
  USUL_TRACE_SCOPE;
  Usul::Threads::Safe::set ( this->mutex(), a, _alpha );
  this->updateTilesAlpha();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add an alpha value (Currently a no-op).
//
///////////////////////////////////////////////////////////////////////////////

void Body::alpha ( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the alpha value. 
//
///////////////////////////////////////////////////////////////////////////////

float Body::alpha() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _alpha;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the alpha values (Currenlty empty map). 
//
///////////////////////////////////////////////////////////////////////////////

Body::Alphas Body::alphas() const
{
  USUL_TRACE_SCOPE;
  return Alphas();
}


///////////////////////////////////////////////////////////////////////////////
//
// Return overall extents.
//
///////////////////////////////////////////////////////////////////////////////

Body::Extents Body::extents() const
{
  USUL_TRACE_SCOPE;
  Extents e ( 0, 0, 0, 0 );
  Tiles tiles ( Usul::Threads::Safe::get ( this->mutex(), _topTiles ) );
  for ( Tiles::const_iterator i = tiles.begin(); i != tiles.end(); ++i )
  {
    Tile::RefPtr tile ( *i );
    if ( true == tile.valid() )
    {
      e.expand ( tile->extents() );
    }
  }
  return e;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect only with the tiles (no vector data).
//
///////////////////////////////////////////////////////////////////////////////

bool Body::intersectWithTiles ( const Usul::Math::Vec3d& point0, const Usul::Math::Vec3d& point1, Usul::Math::Vec3d& point )
{
  Tiles tiles ( Usul::Threads::Safe::get ( this->mutex(), _topTiles ) );

  // Make a temporary group to hold the tiles.
  osg::ref_ptr<osg::Group> group ( new osg::Group );
  for ( Tiles::iterator iter = tiles.begin(); iter != tiles.end(); ++iter )
  {
    group->addChild ( (*iter).get() );
  }
  
  // Points to intersect with.
  osg::Vec3d pt0 ( Usul::Convert::Type<Usul::Math::Vec3d,osg::Vec3d>::convert ( point0 ) );
  osg::Vec3d pt1 ( Usul::Convert::Type<Usul::Math::Vec3d,osg::Vec3d>::convert ( point1 ) );
  
  // Make the intersector.
  typedef osgUtil::LineSegmentIntersector Intersector;
  osg::ref_ptr<Intersector> intersector ( new Intersector ( pt0, pt1 ) );
  
  // Declare the pick-visitor.
  typedef osgUtil::IntersectionVisitor Visitor;
  osg::ref_ptr<Visitor> visitor ( new Visitor );
  visitor->setIntersector ( intersector.get() );
  
  // Intersect the scene.
  group->accept ( *visitor );
  
  // Get the hit-list for our line-segment.
  typedef osgUtil::LineSegmentIntersector::Intersections Intersections;
  const Intersections &hits = intersector->getIntersections();
  if ( hits.empty() )
    return false;
  
  // Set the hit.
  osg::Vec3d hit ( intersector->getFirstIntersection().getWorldIntersectPoint() );
  point = Usul::Convert::Type<osg::Vec3d,Usul::Math::Vec3d>::convert ( hit );
  
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the max degree of anisotropic filtering.
//  See http://en.wikipedia.org/wiki/Anisotropic_filtering
//
///////////////////////////////////////////////////////////////////////////////

float Body::maxAnisotropy() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _maxAnisotropy;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the intersection.
//
///////////////////////////////////////////////////////////////////////////////

void Body::intersectionGraphicClear()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _graphic->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the intersection.
//
///////////////////////////////////////////////////////////////////////////////

void Body::intersectionGraphicSet ( double lonMouse, double latMouse, double elevMouse, 
                                    double lonPoint, double latPoint, double elevPoint )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  typedef Minerva::Core::Data::DataObject DataObject;
  typedef Minerva::Core::Data::Line Line;
  typedef Minerva::Core::Data::LineStyle LineStyle;

  // Clear it first.
  _graphic->clear();

  Line::RefPtr line ( new Line );
  Line::Vertices v;
  v.push_back ( Line::Vertex ( lonMouse, latMouse, 0 ) );
  v.push_back ( Line::Vertex ( lonPoint, latPoint, 0 ) );
  line->line ( v );
  line->lineStyle ( LineStyle::create ( Line::Color ( 1, 0, 0, 1 ), 3.0f ) );

  DataObject::RefPtr data ( new DataObject );
  data->addGeometry ( line.get() );
  _graphic->add ( Usul::Interfaces::IUnknown::QueryPtr ( data ) );
}
