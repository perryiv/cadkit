
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A Jupiter to Performer translator.
//
///////////////////////////////////////////////////////////////////////////////

#include "Controllers/CtInline.h"
#include "Controllers/CtTranslation.h"

#include "Database/Jupiter/DbJtDatabase.h"

#include "Database/Performer/DbPfDatabase.h"

#include "Performer/pf.h"
#include "Performer/pr.h"

#include <iostream>

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize Performer.
//
///////////////////////////////////////////////////////////////////////////////

bool initPerformer()
{
  // Initialize Performer.
  int result = ::pfInit();
  if ( 1 != result )
  {
    std::cout << "Failed to initialize Performer, pfInit() returned " << result << std::endl;
    return false;
  }

  // Tell Performer to print all warnings.
  ::pfNotifyLevel ( PFNFY_ALWAYS );

  // No multiprocessing.
  result = ::pfMultiprocess ( 0 );
  if ( 1 != result )
  {
    std::cout << "Failed to set the number of processes, pfMultiprocess(0) returned " << result << std::endl;
    return false;
  }

  // Configure Performer, this finally gets Performer to a usable state.
  result = ::pfConfig();
  if ( 1 != result )
  {
    std::cout << "Failed to configure Performer, pfConfig(0) returned " << result << std::endl;
    return false;
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // Initialize Performer.
  if ( false == initPerformer() )
    return 0;

  // Translate.
  bool result = CadKit::translate ( new CtTranslation, new DbJtDatabase, new DbPfDatabase, argc, argv );

  // Done with Performer.
  ::pfExit();

  // Return the result.
  return ( result ) ? 1 : 0;
}
