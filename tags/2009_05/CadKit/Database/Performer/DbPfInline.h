
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

#ifdef _MSC_VER
# ifndef _CADKIT_COMPILING_DATABASE_PERFORMER_LIBRARY
#  error "This header file is intended for internal Database/Performer code only."
# endif
#endif

#include "DbPfPerformer.h"

#include "Standard/SlMatrix44.h"
#include "Standard/SlRefPtr.h"

#include "Interfaces/IEntityQuery.h"


namespace CadKit {


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
    pfNode *node = cloneMe->clone ( 0 );

    // Should be true.
    SL_ASSERT ( node->isOfType ( pfDCS::getClassType() ) );

    // Assign the group.
    group = reinterpret_cast<pfDCS *> ( node );
  }

  // Otherwise, just make a new one.
  else
  {
    group = new pfDCS;
  }

  if ( group.isNull() )
  {
    return NULL;
  }

  // Set the name.
  group->setName ( query->getName ( entity ).c_str() );

  // Set the matrix if there is one.
  SlMatrix44f T;
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
