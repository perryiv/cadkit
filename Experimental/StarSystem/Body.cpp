
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
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/MinMax.h"
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
  _textureJobs (),
  _frame ( false ),
  _texturesPerFrame ( 0 ),
  _maxTexturesPerFrame ( 10 ),
  _maxLevel ( 50 ),
  _cacheTiles ( false ),
  _splitDistance ( splitDistance ),
  _meshSize ( ms ),
  _useSkirts ( true ),
  _splitCallback ( new StarSystem::Callbacks::PassThrough ),
  _scale ( 1 )
{
  USUL_TRACE_SCOPE;

  // Serialization setup.
  this->_addMember ( "transformation", _transform );
  this->_addMember ( "land_model", _landModel );
  this->_addMember ( "raster_group", _rasters );
  this->_addMember ( "elevation_group", _elevation );
  this->_addMember ( "max_textures_per_frame", _maxTexturesPerFrame );
  this->_addMember ( "max_level", _maxLevel );
  this->_addMember ( "cache_tiles", _cacheTiles );
  this->_addMember ( "split_distance", _splitDistance );
  this->_addMember ( "mesh_size", _meshSize );
  this->_addMember ( "use_skirts", _useSkirts );
  this->_addMember ( "split_callback", _splitCallback );
  this->_addMember ( "scale", _scale );
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

  // Make the image for the tile.
  osg::ref_ptr< osg::Image > image ( Tile::buildRaster ( extents, width, height, level, this->rasterData(), 0x0 ) );
  
  // Make the tile.
  const Usul::Math::Vec4d textureCoords ( 0.0, 1.0, 0.0, 1.0 );
  const MeshSize meshSize ( this->meshSize ( extents ) );
  osg::ref_ptr<Tile> tile ( new Tile ( level, extents, meshSize, textureCoords, _splitDistance, this, image.get() ) );

  // Add tile to the trnsform.
  _transform->addChild ( tile.get() );
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

void Body::rasterAppend ( RasterLayer * layer )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( ( true == _rasters.valid() ) && ( 0x0 != layer ) )
  {
    // Append the layer to the existing group.
    _rasters->append ( layer );

    // Dirty the tiles.
    DirtyTiles dirty ( true, Tile::TEXTURE, layer->extents() );
    osg::ref_ptr<osg::NodeVisitor> visitor ( OsgTools::MakeVisitor<osg::Group>::make ( dirty ) );
    _transform->accept ( *visitor );
  }
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

void Body::latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3f& point ) const
{
  USUL_TRACE_SCOPE;
  if ( true == _landModel.valid() )
    _landModel->latLonHeightToXYZ ( lat, lon, elevation, point );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert x,y,z to lat, lon, height.
//
///////////////////////////////////////////////////////////////////////////////

void Body::xyzToLatLonHeight ( const osg::Vec3& point, double& lat, double& lon, double& elevation ) const
{
  USUL_TRACE_SCOPE;
  if ( true == _landModel.valid() )
    _landModel->xyzToLatLonHeight ( point, lat, lon, elevation );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the job manager for this body.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Jobs::Manager *Body::jobManager()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _manager;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the job manager.
//
///////////////////////////////////////////////////////////////////////////////

void Body::jobManager ( Usul::Jobs::Manager *manager )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _manager = manager;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer just rendered.
//
///////////////////////////////////////////////////////////////////////////////

void Body::preRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  BaseClass::preRender ( caller );

  Guard guard ( this );
  _frame = true;
  _texturesPerFrame = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer is about to render.
//
///////////////////////////////////////////////////////////////////////////////

void Body::postRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  BaseClass::postRender ( caller );

  Guard guard ( this );
  _frame = false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Request texture.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Body::textureRequest ( const Extents &extents, unsigned int level )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 == _manager )
  {
    // Have to throw because there's no value we can return.
    throw std::runtime_error ( "Error 3925869673: Job manager is null" );
  }

  CutImageJob::RefPtr job ( new CutImageJob ( extents, 512, 512, level, _rasters.get() ) );
  _manager->addJob ( job );
  _textureJobs.insert ( TextureJobs::value_type ( job->id(), job ) );

  return job->id();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel request.
//
///////////////////////////////////////////////////////////////////////////////

void Body::textureRequestCancel ( unsigned long id )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle no job manager.
  if ( 0x0 == _manager )
    return;

  TextureJobs::iterator iter = _textureJobs.find ( id );

  if ( iter != _textureJobs.end() )
  {
    CutImageJob::RefPtr job ( iter->second );
    _manager->cancel ( job );
    _textureJobs.erase ( iter );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

osg::Texture2D* Body::texture ( unsigned long id )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  TextureJobs::iterator iter = _textureJobs.find ( id );
  osg::ref_ptr < osg::Texture2D > texture ( 0x0 );

  if ( iter != _textureJobs.end() && _frame && _texturesPerFrame < _maxTexturesPerFrame )
  {
    CutImageJob::RefPtr job ( iter->second );
    if ( job->isDone() && false == job->canceled() )
    {
      texture = job->texture();
      _textureJobs.erase ( iter );
      ++_texturesPerFrame;
    }
  }

  return texture.release();
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

  // Dirty the tiles.
  DirtyTiles dirty ( true, Tile::VERTICES );
  osg::ref_ptr<osg::NodeVisitor> visitor ( OsgTools::MakeVisitor<osg::Group>::make ( dirty ) );
  _transform->accept ( *visitor );
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

void Body::elevationAppend ( RasterLayer * layer )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( ( true == _elevation.valid() ) && ( 0x0 != layer ) )
  {
    // Append the layer to the existing group.
    _elevation->append ( layer );

    // Dirty the tiles.
    DirtyTiles dirty ( true, Tile::VERTICES, layer->extents() );
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
