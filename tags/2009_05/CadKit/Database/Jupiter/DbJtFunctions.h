
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

#include "Standard/SlMatrix44.h"
#include "Standard/SlMaterial.h"

class eaiEntity;
class eaiTransform;
class eaiMaterial;


namespace CadKit
{
  // Get the transformation.
  eaiTransform *getTransform ( eaiEntity *entity );

  // Get the transformation matrix.
  bool getTransform ( const float &negativeZero, const float &positiveZero, eaiEntity *ptr, SlMatrix44f &matrix );

  // Get the material.
  eaiMaterial *getMaterial ( eaiEntity *entity );

  // Get the material.
  bool getMaterial ( const float &negativeZero, const float &positiveZero, eaiEntity *entity, SlMaterialf &mat );
};


#endif // _CADKIT_DATABASE_JUPITER_LIBRARY_HELPER_FUNCTIONS_DATABASE_H_
