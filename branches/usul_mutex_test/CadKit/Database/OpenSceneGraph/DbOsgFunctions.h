
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbOsgFunctions: OpenSceneGraph database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_HELPER_FUNCTIONS_H_
#define _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_HELPER_FUNCTIONS_H_

#include "Interfaces/Enum.h"

#include "Standard/SlMaterial.h"

#include "osg/Geometry"


namespace CadKit
{
  // Set the state's material from the given SlMaterialf.
  void setMaterial ( const SlMaterialf &material, osg::StateSet *state );

  // Convert the binding.
  osg::Geometry::AttributeBinding getBinding ( const VertexBinding &binding );
};


#endif // _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_HELPER_FUNCTIONS_H_
