
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbJtFunctions: Helper functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_JUPITER_LIBRARY_HELPER_FUNCTIONS_DATABASE_H_
#define _CADKIT_DATABASE_JUPITER_LIBRARY_HELPER_FUNCTIONS_DATABASE_H_

#include "Standard/SlMatrix4.h"
#include "Standard/SlMaterial.h"

class eaiEntity;


namespace CadKit
{
// Get the transformation matrix.
bool getTransform ( eaiEntity *ptr, SlMatrix4f &matrix );

// Get the material.
bool getMaterial ( eaiEntity *entity, SlMaterialf &mat );

}; // namespace CadKit


#endif // _CADKIT_DATABASE_JUPITER_LIBRARY_HELPER_FUNCTIONS_DATABASE_H_
