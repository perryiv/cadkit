
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbPfFunctions: Performer database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_PERFORMER_LIBRARY_HELPER_FUNCTIONS_H_
#define _CADKIT_DATABASE_PERFORMER_LIBRARY_HELPER_FUNCTIONS_H_

#ifdef _MSC_VER
# ifndef _CADKIT_COMPILING_DATABASE_PERFORMER_LIBRARY
#  error "This header file is intended for internal Database/Performer code only."
# endif
#endif

#include "Interfaces/Enum.h"
#include "Interfaces/IQueryVertices.h"

#include "Standard/SlMaterial.h"

class pfGeoState;


namespace CadKit
{
  // Set the state's material from the given SlMaterialf.
  void setMaterial ( const SlMaterialf &material, pfGeoState *state );

  // Convert the binding.
  int getBinding ( const VertexBinding &binding );

  // Convert the primitive type.
  int getPrimitiveType ( const VertexSetType &type );
};


#endif // _CADKIT_DATABASE_PERFORMER_LIBRARY_HELPER_FUNCTIONS_H_
