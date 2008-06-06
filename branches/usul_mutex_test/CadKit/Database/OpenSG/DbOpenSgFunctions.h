
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbOpenSgFunctions: OpenSG database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_OPEN_SG_LIBRARY_HELPER_FUNCTIONS_H_
#define _CADKIT_DATABASE_OPEN_SG_LIBRARY_HELPER_FUNCTIONS_H_

#include "Interfaces/Enum.h"

#include "Standard/SlMaterial.h"

#include <OpenSG/OSGGeometry.h>


namespace CadKit
{
  // Set the state's material from the given SlMaterialf.
  void setMaterial ( const SlMaterialf &material, osg::GeometryPtr geometry );
  
  // Apply a default material to the geometry.
  void setDefaultMaterial ( osg::GeometryPtr geometry );
};


#endif // _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_HELPER_FUNCTIONS_H_
