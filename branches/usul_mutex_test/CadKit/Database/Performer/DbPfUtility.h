
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

#ifndef _CADKIT_DATABASE_PERFORMER_LIBRARY_UTILITY_FUNCTIONS_H_
#define _CADKIT_DATABASE_PERFORMER_LIBRARY_UTILITY_FUNCTIONS_H_

#include "DbPfApi.h"


namespace CadKit {


struct DB_PERFORMER_API Performer
{
  // Start up Performer.
  static bool start();

  // Tell Performer you are done with it.
  static void finish();
};


};


#endif // _CADKIT_DATABASE_PERFORMER_LIBRARY_UTILITY_FUNCTIONS_H_
