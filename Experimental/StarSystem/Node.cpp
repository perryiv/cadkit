
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
//  Node class.
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/Node.h"
#include "StarSystem/Visitor.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

#include "ossim/init/ossimInit.h"

using namespace StarSystem;

STAR_SYSTEM_IMPLEMENT_NODE_CLASS ( Node );


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class to initialize and finalize.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  struct Init
  {
    Init()
    {
      ossimInit::instance()->initialize();
    }
    ~Init()
    {
      ossimInit::instance()->finalize();
    }
  } _initializer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Node::Node() : BaseClass(),
  SERIALIZE_XML_INITIALIZER_LIST
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Node::~Node()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Node::_destroy ), "1211799437" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Node::_destroy()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer just rendered.
//
///////////////////////////////////////////////////////////////////////////////

void Node::preRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer is about to render.
//
///////////////////////////////////////////////////////////////////////////////

void Node::postRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
}
