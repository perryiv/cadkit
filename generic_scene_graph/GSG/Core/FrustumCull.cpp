
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The frustum culling class.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/Core/Precompiled.h"
#include "GSG/Core/FrustumCull.h"
#include "GSG/Core/Transform.h"
#include "GSG/Core/Lod.h"
#include "GSG/Core/Camera.h"
#include "GSG/Core/Shape.h"

using namespace GSG;

GSG_IMPLEMENT_CLONE ( FrustumCull );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FrustumCull::FrustumCull ( RenderBin *bin ) : BinBuilder ( bin )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

FrustumCull::FrustumCull ( const FrustumCull &c ) : BinBuilder ( c )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FrustumCull::~FrustumCull()
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  The visit functions.
//
///////////////////////////////////////////////////////////////////////////////

GSG_DEFINE_DELEGATING_VISIT_NODE_FUNCTIONS ( FrustumCull, _cull );


///////////////////////////////////////////////////////////////////////////////
//
//  Cull the node.
//
///////////////////////////////////////////////////////////////////////////////

void FrustumCull::_cull ( Node &n )
{
  // Should never be here.
  ErrorChecker ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cull the shape.
//
///////////////////////////////////////////////////////////////////////////////

void FrustumCull::_cull ( Shape &s )
{
  Lock lock ( this );

  // Insert the accumulated matrix and the shape.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cull the group.
//
///////////////////////////////////////////////////////////////////////////////

void FrustumCull::_cull ( Group &g )
{
  Lock lock ( this );
  this->_traverse ( g ); // Traverse first.
  this->_cull ( static_cast < Node & > ( g ) );
  // TODO.
}
