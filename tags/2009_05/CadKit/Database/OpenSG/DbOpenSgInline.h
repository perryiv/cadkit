
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

#ifndef _CADKIT_DATABASE_OPEN_SG_LIBRARY_INLINE_FUNCTIONS_DATABASE_H_
#define _CADKIT_DATABASE_OPEN_SG_LIBRARY_INLINE_FUNCTIONS_DATABASE_H_

#include "Standard/SlMatrix44.h"
#include "Standard/SlRefPtr.h"

#include "Interfaces/IEntityQuery.h"

// Do not pre-compile. 
#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGSimpleAttachments.h>
#include <OpenSG/OSGTransform.h>


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Create the group.
//
///////////////////////////////////////////////////////////////////////////////

template <class HandleType, class InterfaceType> 
inline osg::TransformPtr createGroup ( HandleType entity, InterfaceType *query, osg::NodePtr cloneMe )
{
  SL_ASSERT ( entity );
  SL_ASSERT ( query );
  
  osg::TransformPtr core;
  if ( cloneMe != osg::NullFC )
  {
    core = osg::TransformPtr::dcast( cloneMe->getCore() );
  }

  // Create a group. Make a clone if we are supposed to.
  osg::TransformPtr group;
  if ( core != osg::NullFC )
    group = osg::TransformPtr::dcast( core->shallowCopy () );
  else
    group = osg::Transform::create();
  if ( !group )
    return osg::NullFC;

  // Set the name.
  beginEditCP ( group );
  osg::setName ( group, query->getName ( entity ) );

  // Set the matrix if there is one.
  SlMatrix44f matrix;
  if ( query->getTransform ( entity, matrix, false ) )
  {
    osg::Matrix m;
    m.setValue( matrix.getValue() );
    group->setMatrix ( m );
  }

  endEditCP ( group );

  // Return the new group.
  return group;
}


}; // namespace CadKit


#endif // _CADKIT_DATABASE_OPEN_SG_LIBRARY_INLINE_FUNCTIONS_DATABASE_H_
