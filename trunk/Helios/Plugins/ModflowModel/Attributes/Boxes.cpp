
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Boxes attribute.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/ModflowModel/Attributes/Boxes.h"
#include "Helios/Plugins/ModflowModel/Constants.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

#include "osg/Group"

#include <algorithm>

using namespace Modflow::Attributes;

USUL_IMPLEMENT_TYPE_ID ( Boxes );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Boxes::Boxes ( const std::string &name, IUnknown *parent ) : BaseClass ( name, parent ),
  _sides()
{
  USUL_TRACE_SCOPE;

  // Do not use a query pointer. When it goes out of scope it deletes us!
  Usul::Interfaces::IUnknown *me ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  _sides.reserve ( 6 );

  _sides.push_back ( new Quads ( "Top",    me, Modflow::Flags::TOP    ) );
  _sides.push_back ( new Quads ( "Bottom", me, Modflow::Flags::BOTTOM ) );
  _sides.push_back ( new Quads ( "East",   me, Modflow::Flags::EAST   ) );
  _sides.push_back ( new Quads ( "West",   me, Modflow::Flags::WEST   ) );
  _sides.push_back ( new Quads ( "North",  me, Modflow::Flags::NORTH  ) );
  _sides.push_back ( new Quads ( "South",  me, Modflow::Flags::SOUTH  ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Boxes::~Boxes()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Boxes::_destroy ), "3568525110" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Boxes::_destroy()
{
  USUL_TRACE_SCOPE;
  _sides.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void Boxes::clear()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _sides.clear();
  BaseClass::clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child at the position.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Interfaces::ITreeNode *Boxes::getChildNode ( unsigned int i ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( ( i >= _sides.size() ) ? 0x0 : _sides.at(i).get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child at the position.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITreeNode *Boxes::getChildNode ( unsigned int i )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( ( i >= _sides.size() ) ? 0x0 : _sides.at(i).get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Boxes::getNumChildNodes() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _sides.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state.
//
///////////////////////////////////////////////////////////////////////////////

void Boxes::setBooleanState ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Set this instance's state.
  this->visible ( state );

  // Set child's state.
  typedef Usul::Interfaces::IBooleanState IBooleanState;
  std::for_each ( _sides.begin(), _sides.end(), std::bind2nd ( std::mem_fun ( &IBooleanState::setBooleanState ), state ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Boxes::buildScene ( Modflow::Model::Layer *layer )
{
  USUL_TRACE_SCOPE;

  // Build a new group.
  GroupPtr group ( new osg::Group );

  // No longer dirty. Do this before we can return.
  this->dirtyState ( false );

  // If we are visible...
  if ( true == this->visible() )
  {
    // Make copy of the quads.
    Sides sides;
    {
      Guard guard ( this->mutex() );
      sides = _sides;
    }

    // Loop through the sides.
    for ( unsigned int i = 0; i < sides.size(); ++i )
    {
      Quads::RefPtr quads ( sides.at(i) );
      if ( true == quads.valid() )
      {
        group->addChild ( quads->buildScene ( layer ) );
      }
    }
  }

  // Set new scene and return it.
  this->_setScene ( group.get() );
  return BaseClass::buildScene ( layer );
}
