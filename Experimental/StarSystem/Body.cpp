
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

#include "StarSystem/Body.h"
#include "StarSystem/DirtyTiles.h"
#include "StarSystem/Tile.h"
#include "StarSystem/Visitor.h"

#include "OsgTools/Convert/MatrixTransform.h"
#include "OsgTools/Group.h"
#include "OsgTools/Visitor.h"

#include "Usul/Adaptors/MemberFunction.h"
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

using namespace StarSystem;

USUL_FACTORY_REGISTER_CREATOR ( Body );
STAR_SYSTEM_IMPLEMENT_NODE_CLASS ( Body );


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
  _manager ( manager ),
  _maxLevel ( 50 ),
  _cacheTiles ( false ),
  _splitDistance ( splitDistance ),
  _meshSize ( ms ),
  _useSkirts ( true ),
  _splitCallback ( new StarSystem::Callbacks::PassThrough ),
  _scale ( 1 ),
  _deleteTiles(),
  _topTiles()
{
  USUL_TRACE_SCOPE;

  // Serialization setup.
  this->_addMember ( "transformation", _transform );
  this->_addMember ( "land_model", _landModel );
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
  DirtyTiles dirty ( true, Tile::IMAGE /*, e*/ );
  osg::ref_ptr<osg::NodeVisitor> visitor ( OsgTools::MakeVisitor<osg::Group>::make ( dirty ) );
  _transform->accept ( *visitor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the raster data.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::RefPtr Body::rasterData()
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

CutImageJob::RefPtr Body::textureRequest ( Tile* tile )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 == this->jobManager() )
  {
    // Have to throw because there's no value we can return.
    throw std::runtime_error ( "Error 3925869673: Job manager is null" );
  }

  CutImageJob::RefPtr job ( new CutImageJob ( Tile::RefPtr ( tile ) ) );
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
    DirtyTiles dirty ( true, Tile::VERTICES, e );
    osg::ref_ptr<osg::NodeVisitor> visitor ( OsgTools::MakeVisitor<osg::Group>::make ( dirty ) );
    _transform->accept ( *visitor );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation data.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::RefPtr Body::elevationData()
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

StarSystem::Callbacks::SplitCallback::RefPtr Body::splitCallback() const
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
  DirtyTiles dirty ( true, Tile::VERTICES | Tile::CHILDREN, Extents( -180, -90, 180, 90 ) );
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
