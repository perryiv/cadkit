
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OpenGL canvas for the GUI-FOX library.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DEMO_CUBE_BUILD_SCENE_H_
#define _CADKIT_DEMO_CUBE_BUILD_SCENE_H_

#include "SceneGraph/Core/SgSeparator.h"
#include "SceneGraph/Core/SgMaterial.h"
#include "SceneGraph/Core/SgTranslation.h"
#include "SceneGraph/Core/SgRotation.h"
#include "SceneGraph/Core/SgCube.h"


namespace CadKit
{
namespace Demos
{

///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene for the cube demo programs.
//
///////////////////////////////////////////////////////////////////////////////

void buildScene ( const unsigned int &numCubes, const bool &nested, SgGroup &group )
{
  // Seed the random-number generator with current time so that
  // the numbers will be different every time we run.
  ::srand ( (unsigned int) ::time ( NULL ) );

  // For making random numbers in the range [0,1]
  float normalize = 1.0f / ((float) RAND_MAX);
  #define RANDOM_NUMBER ((float) ::rand()) * normalize

  // Make a separator for our scene and attach it to the given group.
  SgSeparator::Ptr root = new SgSeparator;
  group.addChild ( root );

  // Loop and build the cubes.
  for ( unsigned int i = 0; i < numCubes; ++i )
  {
    // A separator for the i'th group of nodes.
    SgSeparator::Ptr branch = new SgSeparator;
    branch->removePushPopFlags ( SgSeparator::PROJECTION );

    // Add some material to the scene.
	  SgMaterial::Ptr material = new SgMaterial;
    SlVec4f diffuse ( RANDOM_NUMBER, RANDOM_NUMBER, RANDOM_NUMBER, 1.0f );
    material->material.setDiffuse ( diffuse );
	  material->side = SgMaterial::FRONT;
	  branch->addChild ( material );

    // Now the cube.
    SgCube::Ptr cube = new SgCube;
	  cube->setSize ( 10.0f );
	  cube->setCenter ( SlVec3f ( 3.0f, 0.0f, -10.0f ) );
	  branch->addChild ( cube );

    // A rotation about an axis.
    SlVec3f axis ( 1.0f, 1.0f, 1.0f );
    axis.normalize();
    branch->addChild ( new SgRotation ( axis, 10.0f ) );

    // Make a translation.
    SgTranslation::Ptr trans = new SgTranslation;
    trans->setTranslation ( SlVec3f ( cube->getSize() * 2, 0.0f, 0.0f ) );
    branch->addChild ( trans );

    // Add the i'th branch to the root.
    root->addChild ( branch );

    // Create nested groups if we're supposed to.
    if ( nested )
      root = branch;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene for the cube demo programs.
//
///////////////////////////////////////////////////////////////////////////////

SgGroup::Ptr buildScene ( const unsigned int &numCubes, const bool &nested )
{
  SgGroup::Ptr group ( new SgGroup );
  buildScene ( numCubes, nested, *group );
  return group;
}

}; // namespace Demos

}; // namespace CadKit

#endif // _CADKIT_DEMO_CUBE_BUILD_SCENE_H_
