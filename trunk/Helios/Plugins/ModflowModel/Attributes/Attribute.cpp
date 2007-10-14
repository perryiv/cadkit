
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for attributes.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/ModflowModel/Attributes/Attribute.h"
#include "Helios/Plugins/ModflowModel/Constants.h"
#include "Helios/Plugins/ModflowModel/Model/Cell.h"

#include "OsgTools/Group.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

#include "osg/Group"

#include <sstream>

using namespace Modflow::Attributes;

USUL_IMPLEMENT_TYPE_ID ( Attribute );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Attribute, Attribute::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Attribute::Attribute ( const std::string &name, IUnknown *parent ) : BaseClass ( name ),
  _flags    ( Modflow::Flags::VISIBLE | Modflow::Flags::DIRTY ),
  _scene    ( new osg::Group ),
  _parent   ( parent ),
  _cells    (),
  _minColor ( 0.0f, 0.0f, 0.0f, 1.0f ),
  _maxColor ( 0.0f, 0.0f, 0.0f, 1.0f )
{
  USUL_TRACE_SCOPE;
  // Do not reference _parent!
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Attribute::~Attribute()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Attribute::_destroy ), "1670887942" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::_destroy()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  this->clear();
  _scene = 0x0;
  _parent = 0x0; // Do not unreference!
  this->_cellWeakPointersClear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::clear()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _scene = new osg::Group;
  this->_cellWeakPointersClear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Attribute::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;
  switch ( iid )
  {
  case Usul::Interfaces::ITreeNode::IID:
    return static_cast < Usul::Interfaces::ITreeNode* > ( this );
  case Usul::Interfaces::IBooleanState::IID:
    return static_cast < Usul::Interfaces::IBooleanState* > ( this );
  case Usul::Interfaces::IDirtyState::IID:
    return static_cast < Usul::Interfaces::IDirtyState* > ( this );
  case Usul::Interfaces::IStringGridGet::IID:
    return static_cast < Usul::Interfaces::IStringGridGet* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the visibility.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::visible ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Don't set to same state.
  if ( this->visible() == state )
    return;

  // Set the visible flag.
  _flags = Usul::Bits::set ( _flags, Modflow::Flags::VISIBLE, state );

  // Set the flags that says we are modified and dirty.
  this->dirtyState ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the visibiity.
//
///////////////////////////////////////////////////////////////////////////////

bool Attribute::visible() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return Usul::Bits::has ( _flags, Modflow::Flags::VISIBLE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::dirtyState ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Don't set to same state.
  if ( this->dirtyState() == state )
    return;

  // Set local flag.
  _flags = Usul::Bits::set ( _flags, Modflow::Flags::DIRTY, state );

  // If we're dirty then set parent's state.
  if ( true == state )
  {
    Usul::Interfaces::IDirtyState::QueryPtr dirty ( _parent );
    if ( true == dirty.valid() )
      dirty->dirtyState ( state );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Attribute::dirtyState() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return Usul::Bits::has ( _flags, Modflow::Flags::DIRTY );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::_setScene ( osg::Group *group )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Set the pointer.
  _scene = group;

  // If we have a valid scene...
  if ( true == _scene.valid() )
  {
    // Make sure the bounds are dirty.
    _scene->dirtyBound();

    // Set this instance as the user data.
    _scene->setUserData ( new UserData ( this ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Attribute::buildScene ( Modflow::ModflowDocument *, Modflow::Model::Layer * )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _scene.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child at the position.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Interfaces::ITreeNode *Attribute::getChildNode ( unsigned int i ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child at the position.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITreeNode *Attribute::getChildNode ( unsigned int which )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Attribute::getNumChildNodes() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the node name.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::setTreeNodeName ( const std::string &s )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  BaseClass::name ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the node name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Attribute::getTreeNodeName() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return this->name();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::setBooleanState ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  this->visible ( state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the node name.
//
///////////////////////////////////////////////////////////////////////////////

bool Attribute::getBooleanState() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return this->visible();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append internal state to list.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::getStringGrid ( Usul::Interfaces::IStringGridGet::StringGrid &data ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  data.push_back ( BaseClass::makeStringRow ( this->className(), this->name() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the weak pointers.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::_cellWeakPointersClear()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _cells.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reserve the weak pointers.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::_cellWeakPointersReserve ( unsigned int size )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _cells.reserve ( size );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append to the weak pointers.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::_cellWeakPointersAppend ( Modflow::Model::Cell *cell, unsigned int num )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  for ( unsigned int i = 0; i < num; ++i )
  {
    _cells.push_back ( Cells::value_type ( cell ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Trim the sequence.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::_cellWeakPointersTrim()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  Cells ( _cells ).swap ( _cells );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the cell from the vertex.
//
///////////////////////////////////////////////////////////////////////////////

Modflow::Model::Cell *Attribute::getCellAtVertex ( unsigned int which )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( ( which < _cells.size() ) ? dynamic_cast < Modflow::Model::Cell * > ( _cells.at(which).get() ) : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the min color.
//
///////////////////////////////////////////////////////////////////////////////

Attribute::Color Attribute::minColor() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _minColor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the min color.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::minColor ( const Color &c )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _minColor = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the max color.
//
///////////////////////////////////////////////////////////////////////////////

Attribute::Color Attribute::maxColor() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _maxColor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the max color.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::maxColor ( const Color &c )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _maxColor = c;
}
