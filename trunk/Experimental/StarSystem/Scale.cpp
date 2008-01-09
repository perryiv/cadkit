
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
//  Visitor class for scaling the scene.
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/Scale.h"
#include "StarSystem/Body.h"

#include "Usul/Trace/Trace.h"

using namespace StarSystem;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Scale::Scale ( double scale ) : BaseClass(),
  _scale ( scale )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Scale::~Scale()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the node.
//
///////////////////////////////////////////////////////////////////////////////

void Scale::visit ( Body &body )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Call the base class's function.
  BaseClass::visit ( body );
}
