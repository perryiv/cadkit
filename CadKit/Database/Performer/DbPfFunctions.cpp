
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

#include "DbPfPrecompiled.h"
#include "DbPfFunctions.h"

#include "Standard/SlMaterial.h"
#include "Standard/SlRefPtr.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Performer/pr/pfGeoState.h"
# include "Performer/pr/pfMaterial.h"
# include "Performer/pr/pfGeoSet.h"
#endif


namespace CadKit
{


///////////////////////////////////////////////////////////////////////////////
//
//  DbPfFunctions: Performer database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

void setMaterial ( const SlMaterialf &material, pfGeoState *state )
{
  SL_ASSERT ( 0 != material.getValid() );
  SL_ASSERT ( state );

  // Make a material.
  SlRefPtr<pfMaterial> mat = new pfMaterial;
  if ( mat.isNull() )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return;
  }

  // Set the colors that are valid.

  // Ambient.
  if ( material.isValid ( SlMaterialf::AMBIENT ) )
  {
    const SlVec4f &v = material.getAmbient();
    mat->setColor ( PFMTL_AMBIENT, v[0], v[1], v[2] );
  }

  // Diffuse.
  if ( material.isValid ( SlMaterialf::DIFFUSE ) )
  {
    const SlVec4f &v = material.getDiffuse();
    mat->setColor ( PFMTL_DIFFUSE, v[0], v[1], v[2] );

    // If the diffuse color's alpha is less than 1 then we have a transparency.
    if ( v[3] < 1.0f )
      state->setMode ( PFSTATE_TRANSPARENCY, PFTR_ON );
  }

  // Specular.
  if ( material.isValid ( SlMaterialf::SPECULAR ) )
  {
    const SlVec4f &v = material.getSpecular();
    mat->setColor ( PFMTL_SPECULAR, v[0], v[1], v[2] );
  }

  // Emissive.
  if ( material.isValid ( SlMaterialf::EMISSIVE ) )
  {
    const SlVec4f &v = material.getEmissive();
    mat->setColor ( PFMTL_EMISSION, v[0], v[1], v[2] );
  }

  // Shininess.
  if ( material.isValid ( SlMaterialf::SHININESS ) )
  {
    // Both Jupiter and Performer use the [0,128] scale for shininess.
    mat->setShininess ( material.getShininess() );
  }

  // TODO, transparency.

  // Set the state's material.
  state->setAttr ( PFSTATE_FRONTMTL, mat );
  //state->setAttr ( PFSTATE_BACKMTL, mat ); // TODO, needed?

  // Turn on lighting. 
  state->setMode ( PFSTATE_ENLIGHTING, PF_ON );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the binding.
//
///////////////////////////////////////////////////////////////////////////////

int getBinding ( const VertexBinding &binding )
{
  switch ( binding )
  {
  case CadKit::BINDING_PER_VERTEX: 
    return PFGS_PER_VERTEX;
  case CadKit::BINDING_PER_PRIMITIVE: 
    return PFGS_PER_PRIM;
  case CadKit::BINDING_OVERALL: 
    return PFGS_OVERALL;
  case CadKit::BINDING_OFF: 
    return PFGS_OFF;
  default:
    SL_ASSERT ( 0 ); // Heads up.
    return PFGS_OFF;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitive mode.
//
///////////////////////////////////////////////////////////////////////////////

int getPrimitiveType ( const VertexSetType &type )
{
  switch ( type )
  {
  case CadKit::LINE_STRIP_SET:
    return PFGS_LINESTRIPS;
  case CadKit::POINT_SET:
    return PFGS_POINTS;
  case CadKit::POLYGON_SET:
    return PFGS_POLYS;
  case CadKit::TRI_STRIP_SET:
    return PFGS_TRISTRIPS;
  case CadKit::TRI_FAN_SET:
    return PFGS_TRIFANS;
  default:
    SL_ASSERT ( 0 );        // What type is this?
    return PFGS_LINESTRIPS; // Play it safe.
  }
}


}; // namespace CadKit
