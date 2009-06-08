
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbPfUtility: Performer convenience functions.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbPfUtility.h"
#include "DbPfPerformer.h"

#include <iostream>


using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Start up Performer.
//
///////////////////////////////////////////////////////////////////////////////

bool Performer::start()
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
//  Tell Performer you are done with it.
//
///////////////////////////////////////////////////////////////////////////////

void Performer::finish()
{
  ::pfExit();
}
