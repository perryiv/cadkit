
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

#include "DbJtVisApiArray.h"

#include "Standard/SlRefPtr.h"
#include "Standard/SlAssert.h"

#include "Interfaces/IMessageNotify.h"
#include "Interfaces/Handles.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "DbJtVisApiHeaders.h"
#endif


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Get the handle from the index.
//
///////////////////////////////////////////////////////////////////////////////

inline LodHandle   makeLodHandle   ( const int &index ) { SL_ASSERT ( index >= 0 ); return (LodHandle)   ( index + 1 ); }
inline ShapeHandle makeShapeHandle ( const int &index ) { SL_ASSERT ( index >= 0 ); return (ShapeHandle) ( index + 1 ); }
inline SetHandle   makeSetHandle   ( const int &index ) { SL_ASSERT ( index >= 0 ); return (SetHandle)   ( index + 1 ); }


///////////////////////////////////////////////////////////////////////////////
//
//  Get the index from the handle.
//
///////////////////////////////////////////////////////////////////////////////

inline int makeLodIndex   ( LodHandle lod )     { SL_ASSERT ( ((int) lod)   > 0 ); return ((int) lod)   - 1; }
inline int makeShapeIndex ( ShapeHandle shape ) { SL_ASSERT ( ((int) shape) > 0 ); return ((int) shape) - 1; }
inline int makeSetIndex   ( SetHandle set )     { SL_ASSERT ( ((int) set)   > 0 ); return ((int) set)   - 1; }


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
//  Handle an entity.
//
///////////////////////////////////////////////////////////////////////////////

template <class NotifyType, class HandleType> inline bool handleEntityStart
  ( NotifyType *entityNotify, HandleType entity, IUnknown *caller, IMessageNotify *messageNotify )
{
  SL_ASSERT ( NULL != entityNotify );
  SL_ASSERT ( NULL != entity );
  SL_ASSERT ( NULL != caller );

  // Let the target know we have a new entity.
  if ( false == entityNotify->startEntity ( entity, caller ) )
  {
    // If there is no messageNotify interface then we failed.
    if ( NULL == messageNotify )
      return false;

    // Otherwise, ask the messageNotify interface.
    return messageNotify->messageNotify ( CadKit::getString ( "Failed to start: %s", CadKit::getName ( entity ).c_str() ), FAILED, IMessageNotify::MESSAGE_ERROR );
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
  ( NotifyType *entityNotify, HandleType entity, IUnknown *caller, IMessageNotify *messageNotify )
{
  SL_ASSERT ( NULL != entityNotify );
  SL_ASSERT ( NULL != entity );
  SL_ASSERT ( NULL != caller );

  // Let the target know we have a new entity.
  if ( false == entityNotify->endEntity ( entity, caller ) )
  {
    // If there is no messageNotify interface then we failed.
    if ( NULL == messageNotify )
      return false;

    // Otherwise, ask the messageNotify interface.
    return messageNotify->messageNotify ( CadKit::getString ( "Failed to end: %s", CadKit::getName ( entity ).c_str() ), FAILED, IMessageNotify::MESSAGE_ERROR );
  }

  // It worked.
  return true;
}


}; // namespace CadKit


#endif // _CADKIT_DATABASE_JUPITER_LIBRARY_INLINE_FUNCTIONS_DATABASE_H_
