
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

#include "StarSystem/SetJobManager.h"
#include "StarSystem/Body.h"
#include "StarSystem/System.h"

#include "Usul/Trace/Trace.h"

using namespace StarSystem;


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

void SetJobManager::visit ( Body &body )
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

void SetJobManager::visit ( System &system )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Set the job manager.
  system.jobManager ( _manager );

  // Call the base class's function.
  BaseClass::visit ( system );
}
