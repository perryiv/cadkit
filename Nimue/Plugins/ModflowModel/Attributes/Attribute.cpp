
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
//  Base class for attributes.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowModel/Attributes/Attribute.h"
#include "ModflowModel/Constants.h"
#include "ModflowModel/Model/Cell.h"

#include "OsgTools/Group.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Convert/Vector4.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Registry/Database.h"
#include "Usul/Strings/Split.h"
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

Attribute::Attribute ( const std::string &name, IUnknown *parent, const RegistryPath &regPath ) : BaseClass ( name ),
  _flags    ( Modflow::Flags::VISIBLE | Modflow::Flags::DIRTY ),
  _scene    ( new osg::Group ),
  _parent   ( parent ),
  _cells    (),
  _minColor ( 0.0f, 0.0f, 0.0f, 1.0f ),
  _maxColor ( 0.0f, 0.0f, 0.0f, 1.0f ),
  _registryPath ( regPath )
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

  // Are we visible?
  if ( false == _registryPath.empty() )
  {
    Usul::Registry::Node &reg ( Usul::Registry::Database::instance()[_registryPath]["visible"] );
    reg.set ( this->visible() );
  }

  // Cleanup...
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
    return BaseClass::queryInterface ( iid );
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
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

Attribute::GroupPtr Attribute::_getScene()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _scene;
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

osg::Group *Attribute::buildScene ( Modflow::ModflowDocument *, Modflow::Model::Layer *, IUnknown * )
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

Usul::Interfaces::ITreeNode::RefPtr Attribute::getChildNode ( unsigned int which )
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
  this->dirtyState ( true );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Update this attribute.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::update ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the parent.
//
///////////////////////////////////////////////////////////////////////////////

Attribute::IUnknown *Attribute::_getParent()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _parent;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the registry path.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::registryPath ( const RegistryPath &path )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _registryPath = path;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::color ( const std::string &s )
{
  USUL_TRACE_SCOPE;

  // Get the parts.
  typedef std::vector<std::string> Strings;
  Strings v;
  Usul::Strings::split ( s, ' ', false, v );

  // Initialize color.
  Color color ( 0.5f * ( _minColor + _maxColor ) );

  // Max color value.
  const float maxValue ( 255.0f );

  // Copy the channels.
  if ( v.size() >= 3 )
  {
    color[0] = Usul::Convert::Type<std::string,Color::value_type>::convert ( v.at(0) ) / maxValue;
    color[1] = Usul::Convert::Type<std::string,Color::value_type>::convert ( v.at(1) ) / maxValue;
    color[2] = Usul::Convert::Type<std::string,Color::value_type>::convert ( v.at(2) ) / maxValue;
  }

  // Alpha is optional.
  if ( 4 == v.size() )
  {
    color[3] = Usul::Convert::Type<std::string,Color::value_type>::convert ( v.at(3) ) / maxValue;
  }

  // Clamp in range.
  color[0] = Usul::Math::minimum ( maxValue, Usul::Math::maximum ( 0.0f, color[0] ) );
  color[1] = Usul::Math::minimum ( maxValue, Usul::Math::maximum ( 0.0f, color[1] ) );
  color[2] = Usul::Math::minimum ( maxValue, Usul::Math::maximum ( 0.0f, color[2] ) );
  color[3] = Usul::Math::minimum ( maxValue, Usul::Math::maximum ( 0.0f, color[3] ) );

  // Set the colors.
  {
    Guard guard ( this );
    this->minColor ( color );
    this->maxColor ( color );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the visible state.
//
///////////////////////////////////////////////////////////////////////////////

void Attribute::initVisibleState()
{
  USUL_TRACE_SCOPE;

  Usul::Registry::Node &reg ( Usul::Registry::Database::instance()[_registryPath]["visible"] );
  const bool visible ( reg.get<bool> ( true ) );
  this->setBooleanState ( visible );
}
