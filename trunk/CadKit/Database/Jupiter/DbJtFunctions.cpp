
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

#include "DbJtPrecompiled.h"
#include "DbJtFunctions.h"
#include "DbJtInline.h"
#include "DbJtVisApiArray.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"
#include "Standard/SlRefPtr.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "DbJtVisApiHeaders.h"
#endif


namespace CadKit
{
// These live in DbJtTraverser for now...
void _incrementPointerReferenceCount ( eaiEntity *p );
void _decrementPointerReferenceCount ( eaiEntity *p );


///////////////////////////////////////////////////////////////////////////////
//
//  Get the transformation matrix.
//
///////////////////////////////////////////////////////////////////////////////

bool getTransform ( eaiEntity *entity, SlMatrix4f &matrix )
{
  SL_PRINT2 ( "In CadKit::getTransform(), entity = %X\n", entity );
  SL_ASSERT ( entity );

  // If we don't have a hierarchy then return.
  if ( false == CadKit::isHierarchy ( entity ) )
    return false;

  // Ask for the transformation (there may not be one).
  eaiTransform *temp = NULL;
  ((eaiHierarchy *) entity)->getTransform ( temp );

  // Auto-release. This assignment will increment it from 0 -> 1.
  SlRefPtr<eaiTransform> transform ( temp );

  // If we didn't get a transform then just return.
  if ( transform.isNull() )
    return false;

  // Get the elements of the matrix.
  DbJtVisApiArray<float> elements;
  transform->getTElements ( elements.getReference() );

  // If we got any elements.
  if ( NULL == elements.getReference() )
    return false;

  // Fill in the given matrix. We assume that the array "elements" 
  // points to 16 valid numbers.
  matrix.setValue ( elements.getReference() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the material from the given entity, if there is one.
//
///////////////////////////////////////////////////////////////////////////////

eaiMaterial *_getMaterial ( eaiEntity *entity )
{
  SL_PRINT2 ( "In CadKit::_getMaterial(), entity = %X\n", entity );
  SL_ASSERT ( entity );

  // Initialize.
  eaiMaterial *material = NULL;

  // Have to explicitly test every concrete type because the API does not 
  // offer a mechanism for checking abstract types, and the base classes do 
  // not have a virtual "getMaterial()".
  switch ( entity->typeID() )
  {
  case eaiEntity::eaiASSEMBLY:
  case eaiEntity::eaiPART:
  case eaiEntity::eaiINSTANCE:
    
    ((eaiHierarchy *) entity)->getMaterial ( material );
    break;

  case eaiEntity::eaiLINESTRIPSET:

    ((eaiLineStripSet *) entity)->getMaterial ( material );
    break;

  case eaiEntity::eaiPOINTSET:

    ((eaiPointSet *) entity)->getMaterial ( material );
    break;

  case eaiEntity::eaiPOLYGONSET:

    ((eaiPolygonSet *) entity)->getMaterial ( material );
    break;

//  case eaiEntity::eaiTRIFANSET:
//
//    ((eaiTriFanSet *) entity)->getMaterial ( material );
//    break;

  case eaiEntity::eaiTRISTRIPSET:

    ((eaiTriStripSet *) entity)->getMaterial ( material );
    break;
  }

  // Return the material, which may still be null.
  return material;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the material.
//
///////////////////////////////////////////////////////////////////////////////

bool getMaterial ( eaiEntity *entity, SlMaterialf &mat )
{
  SL_PRINT2 ( "In CadKit::getMaterial(), entity = %X\n", entity );
  SL_ASSERT ( entity );

  // Initialize.
  bool success ( false );
  mat.setValid ( 0 );

  // Ask for the material (there may not be one). 
  // This assignment will increment it from 0 -> 1.
  SlRefPtr<eaiMaterial> material = CadKit::_getMaterial ( entity );
  if ( material.isNull() )
    return false;

  // Get the components of the material.
  const float BAD_SHININESS ( -999.0f ); // From DirectModel toolkit's example5.
  DbJtVisApiArray<float> a, d, sp, e;
  float sh ( BAD_SHININESS );

  // Ambient.
  material->getAmbientColor ( a.getReference() );
  if ( a.getReference() )
  {
    mat.setAmbient ( SlVec4f ( a[0], a[1], a[2], a[3] ) );
    success = true;
  }

  // Diffuse.
  material->getDiffuseColor ( d.getReference() );
  if ( d.getReference() )
  {
    mat.setDiffuse ( SlVec4f ( d[0], d[1], d[2], d[3] ) );
    success = true;
  }

  // Specular.
  material->getSpecularColor ( sp.getReference() );
  if ( sp.getReference() )
  {
    mat.setSpecular ( SlVec4f ( sp[0], sp[1], sp[2], sp[3] ) );
    success = true;
  }

  // Emissive.
  material->getEmissionColor ( e.getReference() );
  if ( e.getReference() )
  {
    mat.setEmissive ( SlVec4f ( e[0], e[1], e[2], e[3] ) );
    success = true;
  }

  // Shininess.
  material->getShininess ( sh );
  if ( BAD_SHININESS != sh )
  {
    mat.setShininess ( sh );
    success = true;
  }

  // Did it work?
  return success;
}


}; // namespace CadKit
