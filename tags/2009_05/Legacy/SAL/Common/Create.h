
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Template classes and functions that help construct objects.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_SAL_CREATE_OBJECTS_H_
#define _OSG_SAL_CREATE_OBJECTS_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Cast/Cast.h"


namespace SAL {
namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct to do a dynamic cast.
//
///////////////////////////////////////////////////////////////////////////////

struct DynamicCast
{
  template < class SceneBaseType, class SceneNodeType > static void cast ( SceneBaseType *base, SceneNodeType *&node )
  {
    node = dynamic_cast < SceneNodeType * > ( base );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct to safely cast.
//
///////////////////////////////////////////////////////////////////////////////

struct NoRttiCast
{
  template < class SceneBaseType, class SceneNodeType > static void cast ( SceneBaseType *base, SceneNodeType *&node )
  {
    if ( base->isOfType ( SceneNodeType::getClassType() ) )
      node = USUL_UNSAFE_CAST ( SceneNodeType *, base );
    else
      node = 0x0;
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct to create a new node.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class SceneBaseType, 
  class SceneNodeType,
  class NewWrapperType, 
  class InterfaceType,
  class CasterType
>
struct NodeWrapper
{
  static InterfaceType *create ( SceneBaseType *base )
  {
    // See if it is the right type.
    SceneNodeType *node = 0x0;
    CasterType::cast ( base, node );
    if ( node )
    {
      // Allocate a new node-wrapper.
      typedef typename NewWrapperType::ValidRefPtr ValidWrapper;
      ValidWrapper wrapper ( new NewWrapperType ( node ) );

      // Query for the interface.
      typedef typename InterfaceType::ValidQueryPtr ValidInterface;
      ValidInterface in ( wrapper->queryInterface ( InterfaceType::IID ) );

      // Release the pointers and return.
      wrapper.release();
      return in.release();
    }

    // If we get to here then the base-node type was not the desired type.
    return 0x0;
  }
};


}; // namespace Detail
}; // namespace SAL


#endif // _OSG_SAL_CREATE_OBJECTS_H_
