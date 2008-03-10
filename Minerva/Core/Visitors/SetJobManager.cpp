
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
//  Visitor class for setting the job manager.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Visitors/SetJobManager.h"
#include "Minerva/Core/TileEngine/Body.h"
#include "Minerva/Core/TileEngine/System.h"

#include "Usul/Trace/Trace.h"

using namespace Minerva::Core::Visitors;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

SetJobManager::SetJobManager ( Usul::Jobs::Manager *manager ) : BaseClass(),
  _manager ( manager )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

SetJobManager::~SetJobManager()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void SetJobManager::visit ( Minerva::Core::TileEngine::Body &body )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Set the job manager.
  body.jobManager ( _manager );

  // Call the base class's function.
  BaseClass::visit ( body );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void SetJobManager::visit ( Minerva::Core::TileEngine::System &system )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Set the job manager.
  system.jobManager ( _manager );

  // Call the base class's function.
  BaseClass::visit ( system );
}