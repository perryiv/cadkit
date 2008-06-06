
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
  // The modules we use.
  SlRefPtr<CtTranslation> trans  ( new CtTranslation );
  SlRefPtr<DbJtDatabase>  jt     ( new DbJtDatabase );
  SlRefPtr<DbStlDatabase> stl    ( new DbStlDatabase );

  // Make the source only hands the target assemblies and parts instead of 
  // instances of assemblies and parts. Ideally, this should be handled as a 
  // command line argument. 
  SlRefPtr<IUnknown> unknown ( jt->queryInterface ( IUnknown::IID ) );
  SlQueryPtr<ILoadOptions> options ( unknown );
  if ( options.isValid() )
  {
    options->setAssemblyLoadOption ( ILoadOptions::EXPLODE_ASSEMBLY );
    options->setPartLoadOption ( ILoadOptions::EXPLODE_PART );
  }

  // Translate.
  bool result = CadKit::translate ( trans.getValue(), jt.getValue(), stl.getValue(), argc, argv );

  // Return the result.
  return ( result ) ? 1 : 0;
}
