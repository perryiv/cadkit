
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IChildQuery: Interface for querying a child.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_CHILD_QUERY_H_
#define _CADKIT_INTERFACE_CHILD_QUERY_H_

#include "IUnknown.h"
#include "Handles.h"


namespace CadKit
{
template <const unsigned int id, class EntityHandle, class ParentHandle> 
class IChildQuery : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = id };

  // Get the parent of the entity.
  virtual ParentHandle    getParent ( EntityHandle entity ) const = 0;
};


// Common types.
typedef IChildQuery<1033253521,LodHandle,PartHandle>  ILodQuery;
typedef IChildQuery<1033329001,SetHandle,ShapeHandle> ISetQuery;


}; // namespace CadKit


#endif // _CADKIT_INTERFACE_CHILD_QUERY_H_
