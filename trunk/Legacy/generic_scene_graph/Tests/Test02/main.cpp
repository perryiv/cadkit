
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test 2.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/Core/Transform.h"
#include "GSG/Core/Lod.h"
#include "GSG/Core/Factory.h"
#include "GSG/Core/Shape.h"


int main ( int argc, char **argv )
{
  GSG::Transform::Ptr trans ( new GSG::Transform );
  GSG::Lod::Ptr lod ( new GSG::Lod );
  GSG::Factory::Ptr factory ( new GSG::Factory );

  trans->prepend ( lod );
  lod->append ( factory->sphere ( ( argc > 0 ) ? ::atoi ( argv[1] ) : 0 ) );
  lod->append ( factory->sphere() );

  return 0;
}
