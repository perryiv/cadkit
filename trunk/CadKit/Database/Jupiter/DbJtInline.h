
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbJtInline: Inline functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_JUPITER_LIBRARY_INLINE_FUNCTIONS_DATABASE_H_
#define _CADKIT_DATABASE_JUPITER_LIBRARY_INLINE_FUNCTIONS_DATABASE_H_

#include "DbJtPrecompiled.h"
#include "DbJtVisApiArray.h"

#include "Standard/SlRefPtr.h"
#include "Standard/SlAssert.h"

#include "Interfaces/IErrorNotify.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "DbJtVisApiHeaders.h"
#endif


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Check the type.
//
///////////////////////////////////////////////////////////////////////////////

template <class HandleType> inline bool isOfType ( HandleType ptr, const eaiEntity::TypeID &typeId )
{
  return ( ptr && typeId == ((eaiEntity *) ptr)->typeID() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the handle is a hierarchy. We have to jump through these hoops
//  because of the way EAI decided to do runtime type checking.
//
///////////////////////////////////////////////////////////////////////////////

template <class HandleType> inline bool isHierarchy ( HandleType ptr )
{
  return ( 
      CadKit::isOfType ( ptr, eaiEntity::eaiASSEMBLY ) ||
      CadKit::isOfType ( ptr, eaiEntity::eaiPART ) ||
      CadKit::isOfType ( ptr, eaiEntity::eaiINSTANCE ) 
      );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

template <class HandleType> inline std::string getName ( HandleType ptr )
{
  // Declare the name.
  std::string name;

  // If we have a hierarchy then get its name.
  if ( true == CadKit::isHierarchy ( ptr ) )
    name = ((eaiHierarchy *) ptr)->name();

  // Return the name (which may still be blank).
  return name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the transformation matrix.
//
///////////////////////////////////////////////////////////////////////////////

template <class HandleType> inline bool getTransform ( HandleType ptr, float matrix[16] )
{
  // If we don't have a hierarchy then return.
  if ( false == CadKit::isHierarchy ( ptr ) )
    return false;

  // Ask for the transformation (there may not be one).
  eaiTransform *temp = NULL;
  ((eaiHierarchy *) ptr)->getTransform ( temp );

  // Auto-release. This assignment will increment it from 0 -> 1.
  SlRefPtr<eaiTransform> transform ( temp );

  // If we didn't get a transform then just return.
  if ( transform.isNull() )
    return false;

  // Get the elements of the matrix.
  DbJtVisApiArray<float> elements;
  transform->getTElements ( elements.getReference() );

  // For convenience.
  const float *m = elements.getReference();

  // Do we have elements?
  if ( NULL == m )
    return false;

  // Fill in the given matrix.
  std::copy ( m, m + 16, (float *) matrix );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle an entity.
//
///////////////////////////////////////////////////////////////////////////////

template <class NotifyType, class HandleType> inline bool handleEntityStart
  ( NotifyType *notify, HandleType entity, IUnknown *caller, IErrorNotify *controller )
{
  SL_ASSERT ( NULL != notify );
  SL_ASSERT ( NULL != entity );
  SL_ASSERT ( NULL != caller );

  // Let the target know we have a new entity.
  if ( false == notify->startEntity ( entity, caller ) )
  {
    // If there is no controller then we failed.
    if ( NULL == controller )
      return false;

    // Otherwise, ask the controller.
    return controller->errorNotify ( CadKit::getString ( "Failed to start: %s", CadKit::getName ( entity ).c_str() ), FAILED );
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle an entity.
//
///////////////////////////////////////////////////////////////////////////////

template <class NotifyType, class HandleType> inline bool handleEntityEnd
  ( NotifyType *notify, HandleType entity, IUnknown *caller, IErrorNotify *controller )
{
  SL_ASSERT ( NULL != notify );
  SL_ASSERT ( NULL != entity );
  SL_ASSERT ( NULL != caller );

  // Let the target know we have a new entity.
  if ( false == notify->endEntity ( entity, caller ) )
  {
    // If there is no controller then we failed.
    if ( NULL == controller )
      return false;

    // Otherwise, ask the controller.
    return controller->errorNotify ( CadKit::getString ( "Failed to end: %s", CadKit::getName ( entity ).c_str() ), FAILED );
  }

  // It worked.
  return true;
}


}; // namespace CadKit


#endif // _CADKIT_DATABASE_JUPITER_LIBRARY_INLINE_FUNCTIONS_DATABASE_H_
