
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

#include "DbOsgPrecompiled.h"
#include "DbOsgFunctions.h"

#include "Standard/SlMaterial.h"
#include "Standard/SlRefPtr.h"

#include "osg/Material"
#include "osg/BlendFunc"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#endif


namespace CadKit
{


///////////////////////////////////////////////////////////////////////////////
//
//  DbOsgFunctions: OpenSceneGraph database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

void setMaterial ( const SlMaterialf &material, osg::StateSet *state )
{   
  SL_ASSERT ( 0 != material.getValid() );
  SL_ASSERT ( state );
  // Make a material.
  SlRefPtr<osg::Material> mat = new osg::Material;
  if ( mat.isNull() )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return;
  }
  
  // Initialize transparency.
  float alpha = 1.0f;

  // Set the colors that are valid.

  // Ambient.
  if ( material.isValid ( SlMaterialf::AMBIENT ) )
  {
    const SlVec4f &v = material.getAmbient();
    mat->setAmbient ( osg::Material::FRONT, osg::Vec4 ( v[0], v[1], v[2], v[3] ) );
    alpha = v[3];
  }

  // Diffuse.
  if ( material.isValid ( SlMaterialf::DIFFUSE ) )
  {
    const SlVec4f &v = material.getDiffuse();
    mat->setDiffuse ( osg::Material::FRONT, osg::Vec4 ( v[0], v[1], v[2], v[3] ) );
    if(v[3] < alpha)
        alpha = v[3];
  }

  // Specular.
  if ( material.isValid ( SlMaterialf::SPECULAR ) )
  {
    const SlVec4f &v = material.getSpecular();
    mat->setSpecular ( osg::Material::FRONT, osg::Vec4 ( v[0], v[1], v[2], v[3] ) );
  }


  // Emissive.
  if ( material.isValid ( SlMaterialf::EMISSIVE ) )
  {
    const SlVec4f &v = material.getEmissive();
    mat->setEmission ( osg::Material::FRONT, osg::Vec4 ( v[0], v[1], v[2], v[3] ) );
  }

  // Shininess.
  if ( material.isValid ( SlMaterialf::SHININESS ) )
  {
    // Both Jupiter and OpenSceneGraph use the [0,128] scale for shininess.
    mat->setShininess ( osg::Material::FRONT, material.getShininess() );
  }

  // Enable blending and transparency sorting if any colors are transparent.
  if ( alpha < 1.0f )
  {    
    mat->setAlpha(osg::Material::FRONT, alpha);
    osg::ref_ptr<osg::BlendFunc> blendfunc = new osg::BlendFunc;
    state->setAttributeAndModes( blendfunc.get(), osg::StateAttribute::ON );
    state->setRenderingHint ( osg::StateSet::TRANSPARENT_BIN );
  }

  // Set the state's material.
  state->setAttribute ( mat );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the binding.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geometry::AttributeBinding getBinding ( const VertexBinding &binding )
{
  switch ( binding )
  {
  case CadKit::BINDING_PER_VERTEX: 
    return osg::Geometry::BIND_PER_VERTEX;
  case CadKit::BINDING_PER_PRIMITIVE: 
    return osg::Geometry::BIND_PER_PRIMITIVE;
  case CadKit::BINDING_OVERALL: 
    return osg::Geometry::BIND_OVERALL;
  case CadKit::BINDING_OFF: 
    return osg::Geometry::BIND_OFF;
  default:
    SL_ASSERT ( 0 ); // Heads up.
    return osg::Geometry::BIND_OFF;
  }
}


}; // namespace CadKit
