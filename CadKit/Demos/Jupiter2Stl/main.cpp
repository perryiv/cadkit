
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A Jupiter to STL translator.
//
///////////////////////////////////////////////////////////////////////////////

#include "Controllers/CtInline.h"
#include "Controllers/CtTranslation.h"
#include "Database/Jupiter/DbJtDatabase.h"
#include "Database/STL/DbStlDatabase.h"

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Main.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // Make the DMDTk only hand us assemblies and not instances of assemblies
  // (we still get instances of parts). Ideally, this should be handled as 
  // a command line argument.
  DbJtDatabase *jt = new DbJtDatabase;
  SlRefPtr<IUnknown> unknown ( jt->queryInterface ( IUnknown::IID ) );
  SlQueryPtr<ILoadOptions> options ( unknown );
  if ( options.isValid() )
    options->setAssemblyLoadOption ( ILoadOptions::EXPLODE_ASSEMBLY );

  bool result = CadKit::translate ( new CtTranslation, jt, new DbStlDatabase, argc, argv, true );
  return ( result ) ? 1 : 0;
}
