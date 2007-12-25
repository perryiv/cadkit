
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
//  Visitor class for building the scene.
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/BuildScene.h"
#include "StarSystem/System.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Scope/Reset.h"
#include "Usul/Trace/Trace.h"

#include "osg/Group"

using namespace StarSystem;

STAR_SYSTEM_IMPLEMENT_VISITOR_CLASS ( BuildScene );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

BuildScene::BuildScene ( const BuildOptions &options, IUnknown *caller ) : BaseClass(),
  _scene   ( new osg::Group() ),
  _options ( options ),
  _caller  ( caller )
{
  USUL_TRACE_SCOPE;
  _scene->ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

BuildScene::~BuildScene()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &BuildScene::_destroy ), "2031752355" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void BuildScene::_destroy()
{
  USUL_TRACE_SCOPE;
  _scene->unref(); _scene = 0x0;
  _options.clear();
  _caller = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void BuildScene::visit ( Node &node )
{
  USUL_TRACE_SCOPE;
  BaseClass::visit ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void BuildScene::visit ( Body &body )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Add planet to the scene.
  _scene->addChild ( body.scene() );

  // Call the base class's function.
  BaseClass::visit ( body );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void BuildScene::visit ( Group &group )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Make new current group.
  osg::ref_ptr<osg::Group> current ( new osg::Group() );
  _scene->addChild ( current.get() );

  // Make sure the original group gets reset.
  Usul::Scope::Reset<osg::Group *> reset ( _scene, current.get(), _scene );

  // Call the base class's function.
  BaseClass::visit ( group );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void BuildScene::visit ( System &system )
{
  USUL_TRACE_SCOPE;
  BaseClass::visit ( system );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *BuildScene::scene()
{
  USUL_TRACE_SCOPE;
  return _scene;
}
