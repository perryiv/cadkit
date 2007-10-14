
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
#include "Helios/Plugins/ModflowModel/Tools/Clear.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Factory/ObjectFactory.h"
#include "Usul/Factory/TypeCreator.h"
#include "Usul/Interfaces/IDirtyState.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Strings/Format.h"
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

USUL_IMPLEMENT_TYPE_ID ( Layer );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Layer, Layer::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Layer::Layer ( const std::string &name, unsigned int numRows, unsigned int numColumns, const Vec2d &cellSize ) : 
  BaseClass ( name ),
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

  Modflow::Tools::Clear<Rows>::pointers2D ( _rows );
  Modflow::Tools::Clear<Attributes>::pointers1D ( _attributes );

  OsgTools::Group::removeAllChildren ( _root.get() );
  _root->setUserData ( 0x0 );
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
  case Usul::Interfaces::ITreeNode::IID:
    return static_cast < Usul::Interfaces::ITreeNode* > ( this );
  case Usul::Interfaces::IBooleanState::IID:
    return static_cast < Usul::Interfaces::IBooleanState * > ( this );
  case Usul::Interfaces::IDirtyState::IID:
    return static_cast < Usul::Interfaces::IDirtyState * > ( this );
  case Usul::Interfaces::IStringGridGet::IID:
    return static_cast < Usul::Interfaces::IStringGridGet* > ( this );
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

  const unsigned int numRows ( _rows.size() );
  for ( unsigned int i = 0; i < numRows; ++i )
  {
    Row &row ( _rows.at(i) );
    const unsigned int numCols ( row.size() );
    for ( unsigned int j = 0; j < numCols; ++j )
    {
      Cell::RefPtr cell ( row.at(j) );
      if ( true == cell.valid() )
      {
        // Calculate the index.
        const unsigned int index ( ( i * numCols ) + j );

        // Get the top and bottom values.
        const double b ( bottom.at(index) );
        const double t ( top.at(index) );

        // Sanity check.
        if ( t < b )
        {
          // Print warning and keep going.
          std::cout << Usul::Strings::format 
            ( "Warning 9516028000: cell's top value less than bottom.", 
              " Layer: ", this->name(), 
              ", Row: ", i, 
              ", Column: ", j, 
              ", Index: ", index, 
              ", Top: ", t,
              ", Bottom: ", b );
          std::cout << std::endl;
        }

        // Set top and bottom value.
        cell->bottom ( b );
        cell->top ( t );
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
  _root->setUserData ( new UserData ( this ) );
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
  {
    USUL_ASSERT ( false );
    return;
  }

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

  // The bounding sphere is dirty.
  _root->dirtyBound();
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

        // Static cast needed to compile with gcc
        unsigned int size ( static_cast < unsigned int > ( v.size() ) );

        // Resize.
        v.resize ( Usul::Math::maximum ( size, timeStep + 1 ) );

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


///////////////////////////////////////////////////////////////////////////////
//
//  Append internal state to list.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::getStringGrid ( StringGrid &data ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  data.push_back ( Attribute::makeStringRow ( this->name() ) );
}
