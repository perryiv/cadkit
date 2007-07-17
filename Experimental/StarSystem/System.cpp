
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

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

#include "osg/Group"

#include <algorithm>

using namespace StarSystem;

STAR_SYSTEM_IMPLEMENT_NODE_CLASS ( System );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

System::System() : BaseClass(),
  _body       ( new Body() ),
  _satellites ( new Group() )
{
  USUL_TRACE_SCOPE;
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
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *System::buildScene ( const System::BuildOptions &options, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  osg::ref_ptr<osg::Group> group ( new osg::Group() );
  group->addChild ( ( true == _body.valid() ) ?       ( _body->buildScene       ( options, caller ) ) : ( new osg::Group() ) );
  group->addChild ( ( true == _satellites.valid() ) ? ( _satellites->buildScene ( options, caller ) ) : ( new osg::Group() ) );
  return group.release();
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
