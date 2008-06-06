
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A Jupiter to OpenSceneGraph translator.
//
///////////////////////////////////////////////////////////////////////////////

#include "Controllers/CtInline.h"
#include "Controllers/CtTranslation.h"

#include "Database/Jupiter/DbJtDatabase.h"

#include "Database/OpenSceneGraph/DbOsgDatabase.h"

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Main.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // The modules we use.
  SlRefPtr<CtTranslation> trans ( new CtTranslation );
  SlRefPtr<DbJtDatabase>  jt    ( new DbJtDatabase );
  SlRefPtr<DbOsgDatabase> osg   ( new DbOsgDatabase );

  // Translate.
  bool result = CadKit::translate ( trans.getValue(), jt.getValue(), osg.getValue(), argc, argv );

  // Return the result.
  return ( result ) ? 1 : 0;
}
