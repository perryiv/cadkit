
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

#include "DbOpenSgPrecompiled.h"
#include "DbOpenSgFunctions.h"

#include "Standard/SlMaterial.h"
#include "Standard/SlRefPtr.h"

#include "OpenSG/OSGChunkMaterial.h"
#include "OpenSG/OSGMaterialChunk.h"


namespace CadKit
{


///////////////////////////////////////////////////////////////////////////////
//
//  DbOsgFunctions: OpenSceneGraph database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

void setMaterial ( const SlMaterialf &material, osg::GeometryPtr geom )
{
  SL_ASSERT ( 0 != material.getValid() );
  SL_ASSERT ( 0 != geom.getCPtr() );

  // Make a material.
  osg::ChunkMaterialPtr mat = osg::ChunkMaterial::create();
  if ( !mat )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return;
  }
  
  osg::MaterialChunkPtr chunk = osg::MaterialChunk::create();
  if ( !chunk )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return;
  }

  // Set the colors that are valid.

  // Ambient.
  if ( material.isValid ( SlMaterialf::AMBIENT ) )
  {
    const SlVec4f &v = material.getAmbient();
    chunk->setAmbient ( osg::Color4f ( v[0], v[1], v[2], v[3] ) );
  }

  // Diffuse.
  if ( material.isValid ( SlMaterialf::DIFFUSE ) )
  {
    const SlVec4f &v = material.getDiffuse();
    chunk->setDiffuse ( osg::Color4f ( v[0], v[1], v[2], v[3] ) );
  }

  // Specular.
  if ( material.isValid ( SlMaterialf::SPECULAR ) )
  {
    const SlVec4f &v = material.getSpecular();
    chunk->setSpecular ( osg::Color4f ( v[0], v[1], v[2], v[3] ) );
  }

  // Emissive.
  if ( material.isValid ( SlMaterialf::EMISSIVE ) )
  {
    const SlVec4f &v = material.getEmissive();
    chunk->setEmission ( osg::Color4f ( v[0], v[1], v[2], v[3] ) );
  }

  // Shininess.
  if ( material.isValid ( SlMaterialf::SHININESS ) )
  {
    // Both Jupiter and OpenSceneGraph use the [0,128] scale for shininess.
    chunk->setShininess ( material.getShininess() );
  }
  
  mat->addChunk(chunk);

  // Set the state's material.
  geom->setMaterial ( mat );
}


void setDefaultMaterial ( osg::GeometryPtr geometry )
{
  SlMaterialf material;
  material.setDiffuse( SlVec4f( 1, 1, 1, 1 ) );
  CadKit::setMaterial ( material, geometry );
}


}; // namespace CadKit
