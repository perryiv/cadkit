
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all renderers.
//
///////////////////////////////////////////////////////////////////////////////

#include "Display/Render/Renderer.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

using namespace Display::Render;

USUL_IMPLEMENT_TYPE_ID ( Renderer );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

Renderer::Renderer() : BaseClass()
{
  USUL_TRACE_SCOPE;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

Renderer::~Renderer()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Renderer::_destroy ), "3385365210" );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
/////////////////////////////////////////////////////////////////////////////

void Renderer::_destroy()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-render notifcation.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::_preRender()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post-render notifcation.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::_postRender()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::_render ( osg::Node * )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::render ( osg::Node *node )
{
  USUL_TRACE_SCOPE;

  // Make sure pre- and post-render are called.
  Usul::Scope::Caller::RefPtr preAndPostRender ( Usul::Scope::makeCaller 
    ( Usul::Adaptors::memberFunction ( this, &Renderer::_preRender  ),
      Usul::Adaptors::memberFunction ( this, &Renderer::_postRender ) ) );

  // Render the scene.
  this->_render ( node );
}