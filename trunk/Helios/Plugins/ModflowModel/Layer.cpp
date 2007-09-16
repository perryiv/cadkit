
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A layer of modflow cells.
//
///////////////////////////////////////////////////////////////////////////////

#include "Layer.h"
#include "BuildScene.h"
#include "Constants.h"

#include "Usul/Bits/Bits.h"
#include "Usul/Functions/GUID.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Trace/Trace.h"

#include "OsgTools/Callbacks/SortBackToFront.h"
#include "OsgTools/State/StateSet.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Material"

#include <algorithm>
#include <functional>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Layer, Layer::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Layer::Layer ( const std::string &name, unsigned int numRows, unsigned int numColumns, const Vec2d &cellSize ) : 
  BaseClass(),
  _rows ( numRows ),
  _cellSize ( cellSize ),
  _above ( 0x0 ),
  _below ( 0x0 ),
  _guid ( Usul::Functions::GUID::generate() )
{
  USUL_TRACE_SCOPE;

  // Set the name.
  BaseClass::name ( name );

  // Make the cells and set their coordinates.
  const double maxY ( cellSize[1] * numRows );
  for ( unsigned int i = 0; i < numRows; ++i )
  {
    Row &row ( _rows.at(i) );
    row.reserve ( numColumns );
    const double y ( maxY - ( cellSize[1] * i ) );
    for ( unsigned int j = 0; j < numColumns; ++j )
    {
      // Determine cell's x and y coordinates.
      const double x ( cellSize[0] * j );

      // Cell is inactive by default.
      row.push_back ( new Cell ( x, y ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Layer::~Layer()
{
  USUL_TRACE_SCOPE;
  this->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the layer.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::clear()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _above = 0x0;
  _below = 0x0;
  _rows.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Layer::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;
  switch ( iid )
  {
  case Usul::Interfaces::ILayer::IID:
    return static_cast < Usul::Interfaces::ILayer* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set cell's tops and bottoms.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::zRange ( const Data &top, const Data &bottom )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  unsigned int index ( 0 );
  for ( Rows::iterator i = _rows.begin(); i != _rows.end(); ++i )
  {
    Row &row ( *i );
    for ( Row::iterator j = row.begin(); j != row.end(); ++j )
    {
      Cell::RefPtr cell ( *j );
      if ( true == cell.valid() )
      {
        const double b ( bottom.at(index) );
        const double t ( top.at(index) );
        USUL_ASSERT ( t >= b );
        cell->bottom ( b );
        cell->top ( t );
        ++index;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *Layer::buildScene ( unsigned int flags, double scaleFactor, Unknown *caller ) const
{
  USUL_TRACE_SCOPE;
  osg::ref_ptr<osg::Group> group ( new osg::Group );
  group->addChild ( this->_buildCubes ( flags, scaleFactor, caller ) );
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build cubes for the given data set.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *Layer::_buildCubes ( unsigned int flags, double scaleFactor, Unknown *caller ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );

  // Always show both sides.
  OsgTools::State::StateSet::setTwoSidedLighting ( geode.get(), true );

  const Vec2ui gridSize ( this->gridSize() );
  const unsigned int numCells ( gridSize[0] * gridSize[1] );

  // Punt if we don't at least have a 2x2 grid.
  if ( gridSize[0] < 2 || gridSize[1] < 2 )
    return geode.release();

  osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
  vertices->reserve ( numCells * 24 ); // 4 on each face.

  osg::ref_ptr<osg::Vec3Array> normals ( new osg::Vec3Array );
  normals->reserve ( vertices->size() ); // Need same number for "fast path".

  // Determine cell spacing.
  const osg::Vec2d halfSize ( 0.5 * scaleFactor * _cellSize[0], 0.5 * scaleFactor * _cellSize[1] );

  // Don't draw the bottom if there is a cell below us and we're supposed to draw the tops.
  const unsigned int draw ( ( ( true == _below.valid() ) && Usul::Bits::has ( flags, Cell::Draw::TOP ) ) ? Usul::Bits::remove ( flags, Cell::Draw::BOTTOM ) : flags );

  // Make the cells.
  for ( unsigned int i = 0; i < gridSize[0]; ++i )
  {
    const Row &row ( _rows.at(i) );
    for ( unsigned int j = 0; j < gridSize[1]; ++j )
    {
      // Is the cell valid?
      const Cell::RefPtr &cell ( row.at(j) );
      if ( true == cell.valid() )
      {
        // Add the boundary.
        if ( Usul::Bits::has ( flags, Cell::Draw::BOUNDS ) )
        {
          BuildScene::addQuads ( draw, cell->x(), cell->y(), cell->top(), cell->bottom(), halfSize, vertices.get(), normals.get() );
        }

        // Add the starting head.
        if ( Usul::Bits::has ( flags, Cell::Draw::HEADS ) )
        {
          // If there is a cell below us then skip.
          const bool skip ( ( true == _below.valid() ) && ( 0x0 != _below->cellBelow ( i, j ) ) );
          if ( false == skip )
          {
            // Get the heads.
            const Cell::Vector &heads ( cell->vector ( Modflow::Names::HEAD ) );
            if ( false == heads.empty() )
            {
              BuildScene::addQuads ( draw, cell->x(), cell->y(), heads.at(0), cell->bottom(), halfSize * scaleFactor, vertices.get(), normals.get() );
            }
          }
        }
      }
    }
  }

  // We often skip many cells above.
  vertices->trim();
  normals->trim();

  // Make geometry.
  osg::ref_ptr<osg::Geometry> geom ( new osg::Geometry );
  geom->setVertexArray ( vertices.get() );
  geom->setNormalArray ( normals.get() );
  geom->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );

  // Turn both of these on because vertex buffer, when available, 
  // override display-list settings.
  geom->setUseDisplayList ( true );
  geom->setUseVertexBufferObjects ( true );

  // Add primitive set.
  geom->addPrimitiveSet ( new osg::DrawArrays ( osg::DrawArrays::QUADS, 0, vertices->size() ) );
  geode->addDrawable ( geom.get() );

#if 0 // Transparency

  osg::ref_ptr < osg::Material > mat ( new osg::Material );
  osg::Vec4f color ( 51.0f / 255.0f, 204.0f / 255.0f, 204.0f / 255.0f, 1.0f );
  mat->setAmbient ( osg::Material::FRONT_AND_BACK, color );
  mat->setDiffuse ( osg::Material::FRONT_AND_BACK, color );
  mat->setTransparency ( osg::Material::FRONT_AND_BACK, 0.5f );

  osg::ref_ptr<osg::StateSet> ss ( geode->getOrCreateStateSet() );
  ss->setAttributeAndModes ( mat.get() );
  ss->setRenderingHint ( osg::StateSet::TRANSPARENT_BIN );
  ss->setMode ( GL_BLEND, osg::StateAttribute::ON );

  geode->setCullCallback ( new OsgTools::Callbacks::SortBackToFront );
  geom->setUseDisplayList ( false );

#endif

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the grid size.
//
///////////////////////////////////////////////////////////////////////////////

Layer::Vec2ui Layer::gridSize() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  const unsigned int numRows ( _rows.size() );
  const unsigned int numCols ( ( numRows > 0 ) ? _rows.at(0).size() : 0 );

  return Vec2ui ( numRows, numCols );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Purge all the cells that are outside the bounds.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::purge ( const Data &bounds )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  unsigned int index ( 0 );
  for ( Rows::iterator i = _rows.begin(); i != _rows.end(); ++i )
  {
    Row &row ( *i );
    for ( Row::iterator j = row.begin(); j != row.end(); ++j )
    {
      // Delete it if we should.
      if ( 0 == static_cast<unsigned int> ( bounds.at(index) ) )
      {
        // By reference!
        Cell::RefPtr &cell ( *j );
        cell = 0x0;
      }
      ++index;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set all the values for the given time step.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::vector ( const std::string &name, unsigned int timeStep, const Data &heads )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  unsigned int index ( 0 );
  for ( Rows::iterator i = _rows.begin(); i != _rows.end(); ++i )
  {
    Row &row ( *i );
    for ( Row::iterator j = row.begin(); j != row.end(); ++j )
    {
      // If the cell is invalid then the corresponding given value should be ignored.
      Cell::RefPtr &cell ( *j );
      if ( true == cell.valid() )
      {
        // Get vector and resize (not reserve).
        Cell::Vector &v ( cell->vector ( name ) );
        v.resize ( Usul::Math::maximum ( v.size(), timeStep + 1 ) );

        // Write value to cell's vector.
        const double value ( heads.at ( index ) );
        v.at ( timeStep ) = value;
      }

      // Always increment (not just when the cell is valid).
      ++index;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the cell size.
//
///////////////////////////////////////////////////////////////////////////////

Layer::Vec2d Layer::cellSize() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _cellSize;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the layer.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::above ( Layer *above )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _above = above;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer.
//
///////////////////////////////////////////////////////////////////////////////

Layer *Layer::above()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _above.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer.
//
///////////////////////////////////////////////////////////////////////////////

const Layer *Layer::above() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _above.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the layer.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::below ( Layer *below )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _below = below;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer.
//
///////////////////////////////////////////////////////////////////////////////

Layer *Layer::below()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _below.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer.
//
///////////////////////////////////////////////////////////////////////////////

const Layer *Layer::below() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _below.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the cell.
//
///////////////////////////////////////////////////////////////////////////////

const Cell *Layer::cell ( unsigned int row, unsigned int col ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _rows.at ( row ).at ( col ).get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the cell below.
//
///////////////////////////////////////////////////////////////////////////////

const Cell *Layer::cellBelow ( unsigned int row, unsigned int col ) const
{
  USUL_TRACE_SCOPE;
  const Layer *below ( this->below() );
  return ( ( 0x0 == below ) ? 0x0 : below->cell ( row, col ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the guid.
//
///////////////////////////////////////////////////////////////////////////////

std::string Layer::guid() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _guid;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Layer::name() const
{
  USUL_TRACE_SCOPE;
  return BaseClass::name();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the layer.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::showLayer ( bool b )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the layer shown?
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::showLayer() const
{
  USUL_TRACE_SCOPE;
  return true;
}
