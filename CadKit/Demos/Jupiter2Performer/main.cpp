
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
#include "Database/Performer/DbPfUtility.h"

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Main.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // Initialize Performer.
  if ( false == CadKit::Performer::start() )
    return 0;

  // The modules we use.
  SlRefPtr<CtTranslation> trans ( new CtTranslation );
  SlRefPtr<DbJtDatabase>  jt    ( new DbJtDatabase );
  SlRefPtr<DbPfDatabase>  pf    ( new DbPfDatabase );

  // Translate.
  bool result = CadKit::translate ( trans.getValue(), jt.getValue(), pf.getValue(), argc, argv );

  // Done with Performer.
  CadKit::Performer::finish();

  // Return the result.
  return ( result ) ? 1 : 0;
}
