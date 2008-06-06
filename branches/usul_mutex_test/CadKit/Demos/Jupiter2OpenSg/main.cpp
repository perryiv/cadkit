
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A Jupiter to OpenSG translator.
//
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "Controllers/CtInline.h"
#include "Controllers/CtTranslation.h"

#include "Database/Jupiter/DbJtDatabase.h"

#include "Database/OpenSG/DbOpenSgDatabase.h"

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Main.
//
///////////////////////////////////////////////////////////////////////////////

bool go( int argc, char **argv )
{
  // The modules we use.
  SlRefPtr<CtTranslation>    trans ( new CtTranslation );
  SlRefPtr<DbJtDatabase>     jt    ( new DbJtDatabase );
  SlRefPtr<DbOpenSgDatabase> osg   ( new DbOpenSgDatabase );

  // Translate.
  return CadKit::translate ( trans.getValue(), jt.getValue(), osg.getValue(), argc, argv );
}

int main ( int argc, char **argv )
{
  if ( ! osg::osgInit ( argc, argv ) )
  {
    std::cout << "osgInit() failed" << std::endl;
    return EXIT_FAILURE;
  }
  
  bool result = go( argc, argv );

  osg::osgExit();

  // Return the result.
  return ( result ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
