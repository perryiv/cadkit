
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

#include "Standard/SlMatrix4.h"
#include "Standard/SlRefPtr.h"

#include "Interfaces/IEntityQuery.h"

#include "osg/MatrixTransform" // Do not pre-compile. 


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Create the group.
//
///////////////////////////////////////////////////////////////////////////////

template <class HandleType, class InterfaceType> 
inline SlRefPtr<osg::Group> createGroup ( HandleType entity, InterfaceType *query )
{
  SL_ASSERT ( entity );
  SL_ASSERT ( query );

  // Create a group.
  SlRefPtr<osg::MatrixTransform> group = new osg::MatrixTransform;
  if ( group.isNull() )
    return NULL;

  // Set the name.
  group->setName ( query->getName ( entity ) );

  // Set the matrix if there is one.
  SlMatrix4f matrix;
  if ( true == query->getTransform ( entity, matrix, false ) )
    group->setMatrix ( osg::Matrix ( matrix.getValue() ) );

  // Return the new group.
  return SlRefPtr<osg::Group> ( group );
}


}; // namespace CadKit


#endif // _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_INLINE_FUNCTIONS_DATABASE_H_
