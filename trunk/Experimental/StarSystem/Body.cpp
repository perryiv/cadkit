
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
#include "StarSystem/RasterGroup.h"
#include "StarSystem/ElevationGroup.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "OsgTools/Group.h"
#include "OsgTools/Visitor.h"

#include "osg/MatrixTransform"

#include <limits>
#include <stdexcept>

using namespace StarSystem;

STAR_SYSTEM_IMPLEMENT_NODE_CLASS ( Body );


///////////////////////////////////////////////////////////////////////////////
//
//  Serialization glue for osg::ref_ptr<osg::MatrixTransform>
//
///////////////////////////////////////////////////////////////////////////////

SERIALIZE_XML_DECLARE_MATRIX_4_4_WRAPPER ( osg::Matrixd );
typedef StarSystem::Body::MatrixTransformPtr MatrixTransformPtr;
inline std::ostream &operator << ( std::ostream &out, const MatrixTransformPtr &m )
{
  if ( true == m.valid() )
  {
    out << '\n' << m->getMatrix();
  }
  return out;
}
inline std::istream &operator >> ( std::istream &in, MatrixTransformPtr &m )
{
  osg::Matrixd matrix;
  in >> matrix;
  if ( false == m.valid() )
  {
    m = new osg::MatrixTransform;
  }
  m->setMatrix ( matrix );
  return in;
}
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( MatrixTransformPtr );


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
  _splitCallback ( new StarSystem::Callbacks::PassThrough )
{
  USUL_TRACE_SCOPE;

  // Not using smart pointers.
  _rasters->ref();
  _elevation->ref();

  // Serialization setup.
  this->_addMember ( "transformation", _transform );
  this->_addMember ( "textures_per_frame", _texturesPerFrame );
  this->_addMember ( "max_textures_per_frame", _maxTexturesPerFrame );
  this->_addMember ( "max_level", _maxLevel );
  this->_addMember ( "cache_tiles", _cacheTiles );
  this->_addMember ( "split_distance", _splitDistance );
  this->_addMember ( "mesh_size", _meshSize );
  this->_addMember ( "use_skirts", _useSkirts );
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

  Usul::Pointers::unreference ( _rasters ); _rasters = 0x0;
  Usul::Pointers::unreference ( _elevation ); _elevation = 0x0;
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

  // Make the tile.
  const Usul::Math::Vec4d textureCoords ( 0.0, 1.0, 0.0, 1.0 );
  const MeshSize meshSize ( this->meshSize ( extents ) );
  osg::ref_ptr<Tile> tile ( new Tile ( 0, extents, meshSize, textureCoords, _splitDistance, this ) );

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
  _transform->setMatrix ( osg::Matrix::translate ( c[0], c[1], c[2] ) );
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

  if ( 0x0 != layer )
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
//  Computes the "geodetic" radius for a given latitude in degrees.
//
///////////////////////////////////////////////////////////////////////////////

double Body::geodeticRadius ( double latitude ) const
{
  USUL_TRACE_SCOPE;
  return ( ( true == _landModel.valid() ) ? _landModel->elevation ( 0, latitude ) : 1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the thread pool for this body.
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

  CutImageJob::RefPtr job ( new CutImageJob ( extents, 512, 512, level, _rasters ) );
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
  DirtyTiles dirty ( true, Tile::TEXTURE );
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

  if ( 0x0 != _elevation && 0x0 != layer )
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

RasterLayer* Body::elevationData()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _elevation;
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
