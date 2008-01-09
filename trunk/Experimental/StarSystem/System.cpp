
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
//  System class.
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/System.h"
#include "StarSystem/Visitor.h"
#include "StarSystem/LandModelEllipsoid.h"
#include "StarSystem/LandModelFlat.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

#include "osg/Group"
#include "osg/CoordinateSystemNode"

#include <algorithm>

using namespace StarSystem;

USUL_FACTORY_REGISTER_CREATOR ( System );
STAR_SYSTEM_IMPLEMENT_NODE_CLASS ( System );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

System::System ( Usul::Jobs::Manager *manager ) : BaseClass(),
  _body       ( 0x0 ),
  _satellites ( new Group() ),
  _manager    ( manager )
{
  USUL_TRACE_SCOPE;
  this->_addMember ( "body", _body );
  this->_addMember ( "satellites", _satellites );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

System::~System()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &System::_destroy ), "8999770820" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void System::_destroy()
{
  USUL_TRACE_SCOPE;

  _satellites = 0x0;
  _body = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the satellites.
//
///////////////////////////////////////////////////////////////////////////////

void System::satellites ( Group *group )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _satellites = group;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the satellites.
//
///////////////////////////////////////////////////////////////////////////////

Group *System::satellites()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _satellites.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the satellites.
//
///////////////////////////////////////////////////////////////////////////////

const Group *System::satellites() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _satellites.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the body.
//
///////////////////////////////////////////////////////////////////////////////

void System::body ( Body *body )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _body = body;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the body.
//
///////////////////////////////////////////////////////////////////////////////

Body *System::body()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _body.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the body.
//
///////////////////////////////////////////////////////////////////////////////

const Body *System::body() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _body.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a satellite.
//
///////////////////////////////////////////////////////////////////////////////

void System::add ( Node *node )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != node )
  {
    Guard guard ( this->mutex() );
    if ( true == _satellites.valid() )
    {
      _satellites->add ( node );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the center.
//
///////////////////////////////////////////////////////////////////////////////

void System::center ( Vec3d &c )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  if ( true == _body.valid() )
  {
    _body->center ( c );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the center.
//
///////////////////////////////////////////////////////////////////////////////

System::Vec3d System::center() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( ( true == _body.valid() ) ? _body->center() : System::Vec3d ( 0, 0, 0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer just rendered.
//
///////////////////////////////////////////////////////////////////////////////

void System::preRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  BaseClass::preRender ( caller );

  Guard guard ( this );

  if ( _body.valid() )
    _body->preRender ( caller );

  if ( _satellites.valid() )
    _satellites->preRender( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer is about to render.
//
///////////////////////////////////////////////////////////////////////////////

void System::postRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  BaseClass::postRender ( caller );

  Guard guard ( this );

  if ( _body.valid() )
    _body->postRender ( caller );

  if ( _satellites.valid() )
    _satellites->postRender( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the job manager.
//
///////////////////////////////////////////////////////////////////////////////

void System::jobManager ( Usul::Jobs::Manager *manager )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _manager = manager;
}
