
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
#ifdef _DEBUG
#if 0
  SlRefPtr<CtTranslation> temp ( new CtTranslation );
  temp->execute ( argc, argv, (new DbJtDatabase)->queryInterface ( IUnknown::IID ), (new DbOsgDatabase)->queryInterface ( IUnknown::IID ) );
#endif
#endif

  // Translate.
  bool result = CadKit::translate ( new CtTranslation, new DbJtDatabase, new DbOsgDatabase, argc, argv, true );

  // Return the result.
  return ( result ) ? 1 : 0;
}
