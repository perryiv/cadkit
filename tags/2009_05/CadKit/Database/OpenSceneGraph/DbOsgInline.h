
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbOsgInline: Inline functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_INLINE_FUNCTIONS_DATABASE_H_
#define _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_INLINE_FUNCTIONS_DATABASE_H_

#include "Standard/SlMatrix44.h"
#include "Standard/SlRefPtr.h"

#include "Interfaces/IEntityQuery.h"

#include "osg/MatrixTransform" // Do not pre-compile. 
#include "osg/CopyOp"


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Create the group. VC++ has an internal compiler error if the GroupType
//  is not a template argument, but just an osg::Group.
//
///////////////////////////////////////////////////////////////////////////////

template <class HandleType, class InterfaceType, class GroupType> 
inline SlRefPtr<osg::Group> createGroup ( HandleType entity, InterfaceType *query, GroupType *cloneMe, float scaleFactor = 1.0f )
{
  SL_ASSERT ( entity );
  SL_ASSERT ( query );

  // Create a group. Make a clone If we are supposed to.
  SlRefPtr<osg::MatrixTransform> group;
  if ( cloneMe )
    group = dynamic_cast<osg::MatrixTransform *> ( cloneMe->clone ( osg::CopyOp::SHALLOW_COPY ) );
  else
    group = new osg::MatrixTransform;
  if ( group.isNull() )
    return NULL;

  // Set the name.
  group->setName ( query->getName ( entity ) );

  // Set the matrix if there is one.
  SlMatrix44f matrix;
  if ( true == query->getTransform ( entity, matrix, false ) )
  {
    if( scaleFactor != 1.0f)
      matrix.scale( SlMatrix44f::Vec3( scaleFactor, scaleFactor, scaleFactor ) );
    group->setMatrix ( osg::Matrix ( matrix.getValue() ) );
  }

  // Return the new group.
  return SlRefPtr<osg::Group> ( group );
}


}; // namespace CadKit


#endif // _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_INLINE_FUNCTIONS_DATABASE_H_
