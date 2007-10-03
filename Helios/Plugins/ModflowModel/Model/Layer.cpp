
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

#include "Helios/Plugins/ModflowModel/Model/Layer.h"
#include "Helios/Plugins/ModflowModel/Builders/BuildScene.h"
#include "Helios/Plugins/ModflowModel/Constants.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Factory/ObjectFactory.h"
#include "Usul/Factory/TypeCreator.h"
#include "Usul/Interfaces/IDirtyState.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Trace/Trace.h"

#include "OsgTools/Callbacks/SortBackToFront.h"
#include "OsgTools/Group.h"
#include "OsgTools/State/StateSet.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Material"

#include <algorithm>
#include <functional>

using namespace Modflow::Model;

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
  _root ( new osg::Group ),
  _flags ( Modflow::Flags::DIRTY | Modflow::Flags::VISIBLE ),
  _document(),
  _margin ( 0, 0, 0 ),
  _attributes()
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

  // Clear the layer.
  this->clear();

  // Clear children now.
  _root = 0x0;

  // Set the document.
  _document = static_cast < IDocument * > ( 0x0 );
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
  _attributes.clear();
  OsgTools::Group::removeAllChildren ( _root.get() );
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
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ITreeNode::IID:
    return static_cast < Usul::Interfaces::ITreeNode* > ( this );
  case Usul::Interfaces::IBooleanState::IID:
    return static_cast < Usul::Interfaces::IBooleanState * > ( this );
  case Usul::Interfaces::IDirtyState::IID:
    return static_cast < Usul::Interfaces::IDirtyState * > ( this );
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

osg::Node *Layer::buildScene ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  this->_buildScene ( caller );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::_buildScene ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Should never happen...
  if ( false == _root.valid() )
    _root = new osg::Group;

  // Remove all the existing children.
  OsgTools::Group::removeAllChildren ( _root.get() );

  // No longer dirty. Do this before we can return.
  this->dirtyState ( false );

  // Return now if we're not visible.
  if ( false == this->visible() )
    return;

  // Loop through the attributes.
  for ( Attributes::iterator i = _attributes.begin(); i != _attributes.end(); ++i )
  {
    Attribute::RefPtr a ( *i );
    if ( true == a.valid() )
    {
      // Add the scene to the root.
      _root->addChild ( a->buildScene ( this ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build cubes for the given data set.
//
///////////////////////////////////////////////////////////////////////////////

#if 0
osg::Node *Layer::_buildCubes ( Unknown *caller ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Make the geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );

  // Always show both sides.
  OsgTools::State::StateSet::setTwoSidedLighting ( geode.get(), true );

  const Vec2ui gridSize ( this->gridSize() );
  const unsigned int numCells ( gridSize[0] * gridSize[1] );

  // Punt if we don't have at least a 2x2 grid.
  if ( gridSize[0] < 2 || gridSize[1] < 2 )
    return geode.release();

  osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
  vertices->reserve ( numCells * 24 ); // 4 on each face.

  osg::ref_ptr<osg::Vec3Array> normals ( new osg::Vec3Array );
  normals->reserve ( vertices->size() ); // Need same number for "fast path".

  // Determine cell size.
  const osg::Vec2d halfSize ( ( 0.5 * _cellSize[0] ) - _margin[0], ( 0.5 * _cellSize[1] ) - _margin[1] );

  // Don't draw the bottom if there is a visible cell below us and we're supposed to draw the tops.
  const unsigned int flags ( this->flags() );
  const unsigned int draw ( ( ( true == _below.valid() ) && ( true == _below->visible() ) && Usul::Bits::has ( flags, Modflow::Flags::TOP ) ) ? Usul::Bits::remove ( flags, Modflow::Flags::BOTTOM ) : flags );

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
        if ( Usul::Bits::has ( flags, Modflow::Flags::BOUNDS ) )
        {
          Modflow::Builders::BuildScene::addQuads 
            ( draw, cell->x(), cell->y(), cell->top(), cell->bottom(), halfSize, _margin[2], vertices.get(), normals.get() );
        }

        // Add the starting head.
        if ( Usul::Bits::has ( flags, Modflow::Flags::HEADS ) )
        {
          // If there is a cell below us then skip.
          const bool skip ( ( true == _below.valid() ) && ( 0x0 != _below->cellBelow ( i, j ) ) );
          if ( false == skip )
          {
            // Get the heads.
            const Cell::Vector &heads ( cell->vector ( Modflow::Names::HEAD_LEVELS ) );
            if ( false == heads.empty() )
            {
              Modflow::Builders::BuildScene::addQuads 
                ( draw, cell->x(), cell->y(), heads.at(0), cell->bottom(), halfSize, _margin[2], vertices.get(), normals.get() );
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
#endif

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
//  Get the cell.
//
///////////////////////////////////////////////////////////////////////////////

Cell *Layer::cell ( unsigned int row, unsigned int col )
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
//  Set the visible flag.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::visible ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Don't set to same state.
  if ( this->visible() == state )
    return;

  // Set the visible flag.
  this->flags ( Usul::Bits::set ( this->flags(), Modflow::Flags::VISIBLE, state ) );

  // Set the flags that says we are dirty.
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the layer visible?
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::visible() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return Usul::Bits::has ( this->flags(), Modflow::Flags::VISIBLE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set modification flag.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::modified ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( true == _document.valid() )
  {
    _document->modified ( state );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the document.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::document ( Usul::Interfaces::IUnknown *doc )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _document = doc;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flags.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::flags ( unsigned int bits )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _flags = bits;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flags.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Layer::flags() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _flags;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the margin.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::margin ( double x, double y, double z )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _margin.set ( x, y, z );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the margin.
//
///////////////////////////////////////////////////////////////////////////////

Layer::Vec3d Layer::margin() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _margin;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::dirtyState ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Don't set to same state.
  if ( this->dirtyState() == state )
    return;

  // Set local flag.
  this->flags ( Usul::Bits::set ( this->flags(), Modflow::Flags::DIRTY, state ) );

  // If we're dirty then set document's state.
  if ( true == state )
  {
    Usul::Interfaces::IDirtyState::QueryPtr dirty ( _document );
    if ( true == dirty.valid() )
      dirty->dirtyState ( state );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flags.
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::dirtyState() const
{
  USUL_TRACE_SCOPE;
  return Usul::Bits::has ( this->flags(), Modflow::Flags::DIRTY );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::addAttribute ( Attribute *attribute )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  if ( 0x0 != attribute )
  {
    _attributes.push_back ( attribute );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child at the position.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Interfaces::ITreeNode *Layer::getChildNode ( unsigned int i ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( ( i >= _attributes.size() ) ? 0x0 : _attributes.at(i).get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child at the position.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITreeNode *Layer::getChildNode ( unsigned int i )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( ( i >= _attributes.size() ) ? 0x0 : _attributes.at(i).get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Layer::getNumChildNodes() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _attributes.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the node name.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::setTreeNodeName ( const std::string &s )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  BaseClass::name ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the node name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Layer::getTreeNodeName() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return this->name();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::setBooleanState ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Set this instance's state.
  this->visible ( state );

  // Set child's state.
  typedef Usul::Interfaces::IBooleanState IBooleanState;
  std::for_each ( _attributes.begin(), _attributes.end(), std::bind2nd ( std::mem_fun ( &IBooleanState::setBooleanState ), state ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the node name.
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::getBooleanState() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return this->visible();
}
