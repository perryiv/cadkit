
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Cell boundary attribute.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowModel/Attributes/CellBoundary.h"
#include "ModflowModel/Constants.h"

#include "OsgTools/Group.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "osg/Group"

#include <algorithm>

using namespace Modflow::Attributes;

USUL_IMPLEMENT_TYPE_ID ( CellBoundary );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

CellBoundary::CellBoundary ( const std::string &name, Modflow::Geometry::ID topGeometry, IUnknown *parent, const RegistryPath &regPath ) : BaseClass ( name, parent, regPath ),
  _sides(),
  _group ( new osg::Group ),
  _topGeometry ( topGeometry )
{
  USUL_TRACE_SCOPE;

  // Do not use a query pointer. When it goes out of scope it deletes us!
  Usul::Interfaces::IUnknown *me ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  _sides.reserve ( 6 );

  _sides.push_back ( new Quads ( "Top",    me, _topGeometry,                   regPath ) );
  _sides.push_back ( new Quads ( "Bottom", me, Modflow::Geometry::CELL_BOTTOM, regPath ) );
  _sides.push_back ( new Quads ( "East",   me, Modflow::Geometry::CELL_EAST,   regPath ) );
  _sides.push_back ( new Quads ( "West",   me, Modflow::Geometry::CELL_WEST,   regPath ) );
  _sides.push_back ( new Quads ( "North",  me, Modflow::Geometry::CELL_NORTH,  regPath ) );
  _sides.push_back ( new Quads ( "South",  me, Modflow::Geometry::CELL_SOUTH,  regPath ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

CellBoundary::~CellBoundary()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &CellBoundary::_destroy ), "3568525110" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void CellBoundary::_destroy()
{
  USUL_TRACE_SCOPE;
  _sides.clear();
  _group = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void CellBoundary::clear()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _sides.clear();
  OsgTools::Group::removeAllChildren ( _group.get() );

  BaseClass::clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child at the position.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITreeNode::RefPtr CellBoundary::getChildNode ( unsigned int i )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( ( i >= _sides.size() ) ? 0x0 : _sides.at(i).get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int CellBoundary::getNumChildNodes() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _sides.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state.
//
///////////////////////////////////////////////////////////////////////////////

void CellBoundary::setBooleanState ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Set this instance's state.
  this->visible ( state );

  // Note: do not ever hide this node. Instead, just hide the children below.
  // Otherwise, showing a child will have no effect when the parent is hidden.
  // Keeping this here as a reminder.
  //_group->setNodeMask ( ( true == state ) ? 0xFFFFFFFF : 0 );

  // Set child's state.
  typedef Usul::Interfaces::IBooleanState IBooleanState;
  std::for_each ( _sides.begin(), _sides.end(), std::bind2nd ( std::mem_fun ( &IBooleanState::setBooleanState ), state ) );

  // Ask views to redraw.
  BaseClass::requestActiveDocumentRedraw();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *CellBoundary::buildScene ( Modflow::ModflowDocument *document, Modflow::Model::Layer *layer, IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Just return existing group if we're not dirty.
  //if ( false == this->dirtyState() )
    //return _group.get(); // Not until the bug is fixed...

  // Clear the switch.
  OsgTools::Group::removeAllChildren ( _group.get() );

  // No longer dirty. Do this before we can return.
  this->dirtyState ( false );

  // If we are visible...
  if ( true == this->visible() )
  {
    // Make copy of the quads.
    const Sides sides ( Usul::Threads::Safe::get ( this->mutex(), _sides ) );

    // Loop through the sides.
    for ( unsigned int i = 0; i < sides.size(); ++i )
    {
      Quads::RefPtr quads ( sides.at(i) );
      if ( true == quads.valid() )
      {
        _group->addChild ( quads->buildScene ( document, layer, caller ) );
      }
    }
  }

  // Set new scene and return it.
  this->_setScene ( _group.get() );
  return BaseClass::buildScene ( document, layer, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append internal state to list.
//
///////////////////////////////////////////////////////////////////////////////

void CellBoundary::getStringGrid ( Usul::Interfaces::IStringGridGet::StringGrid &data ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  data.push_back ( BaseClass::makeStringRow ( "Surface", this->name() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the min color.
//
///////////////////////////////////////////////////////////////////////////////

void CellBoundary::minColor ( const Color &c )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  for ( Sides::iterator i = _sides.begin(); i != _sides.end(); ++i )
  {
    Quads::RefPtr quads ( *i );
    if ( true == quads.valid() )
    {
      quads->minColor ( c );
    }
  }
  BaseClass::minColor ( c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the max color.
//
///////////////////////////////////////////////////////////////////////////////

void CellBoundary::maxColor ( const Color &c )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  for ( Sides::iterator i = _sides.begin(); i != _sides.end(); ++i )
  {
    Quads::RefPtr quads ( *i );
    if ( true == quads.valid() )
    {
      quads->maxColor ( c );
    }
  }
  BaseClass::maxColor ( c );
}
