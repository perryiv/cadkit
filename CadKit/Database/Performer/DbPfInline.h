
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbPfInline: Inline functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_PERFORMER_LIBRARY_INLINE_FUNCTIONS_DATABASE_H_
#define _CADKIT_DATABASE_PERFORMER_LIBRARY_INLINE_FUNCTIONS_DATABASE_H_

#include "Standard/SlMatrix4.h"
#include "Standard/SlRefPtr.h"

#include "Interfaces/IEntityQuery.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Performer/pf/pfDCS.h"
#endif


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Create the group. VC++ has an internal compiler error if the GroupType
//  is not a template argument, but just an pfGroup.
//
///////////////////////////////////////////////////////////////////////////////

template <class HandleType, class InterfaceType, class GroupType> 
inline SlRefPtr<pfGroup> createGroup ( HandleType entity, InterfaceType *query, GroupType *cloneMe )
{
  SL_ASSERT ( entity );
  SL_ASSERT ( query );

  // Create a group. Make a clone If we are supposed to.
  SlRefPtr<pfDCS> group;
  if ( cloneMe )
  {
    // Note: docs say argument has to be zero. Also, unlike OSG, this will 
    // copy down to (but not including) the leaf nodes.
    group = dynamic_cast<pfDCS *> ( cloneMe->clone ( 0 ) );
  }
  else
    group = new pfDCS;
  if ( group.isNull() )
    return NULL;

  // Set the name.
  group->setName ( query->getName ( entity ).c_str() );

  // Set the matrix if there is one.
  SlMatrix4f T;
  if ( true == query->getTransform ( entity, T, false ) )
  {
    pfMatrix M ( T(0,0), T(1,0), T(2,0), T(3,0),
                 T(0,1), T(1,1), T(2,1), T(3,1),
                 T(0,2), T(1,2), T(2,2), T(3,2),
                 T(0,3), T(1,3), T(2,3), T(3,3) );
    group->setMat ( M );
  }

  // Return the new group.
  return SlRefPtr<pfGroup> ( group );
}


}; // namespace CadKit


#endif // _CADKIT_DATABASE_PERFORMER_LIBRARY_INLINE_FUNCTIONS_DATABASE_H_
