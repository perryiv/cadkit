
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IQueryVertices: Interface for querying vertices.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_QUERY_PRIM_ORIGINS_H_
#define _CADKIT_INTERFACE_QUERY_PRIM_ORIGINS_H_

#include "IQueryArray.h"
#include "Enum.h"

#include "Standard/SlVec3.h"


namespace CadKit
{
template <const unsigned int id, class EntityHandle, class ArrayType> 
class IQueryPrimOrigins : public IQueryArray<id,ArrayType>
{
public:

  // Inherit from this and define the virtual functions.
  class OriginSetter : public IQueryArray<id,ArrayType>::ArraySetter { };

  // Get the set type.
  virtual bool            getPrimType ( EntityHandle entity, PrimitiveType &type ) const = 0;

  // Get the vertices.
  virtual bool            getPrimOrigins ( EntityHandle entity, OriginSetter &setter ) const = 0;
};


// Common types.
typedef IQueryPrimOrigins<1033359885,PrimHandle,SlVec3f> IQueryPrimOriginsVec3f;
typedef IQueryPrimOrigins<1033359886,PrimHandle,SlVec3d> IQueryPrimOriginsVec3d;


}; // namespace CadKit


#endif // _CADKIT_INTERFACE_QUERY_PRIM_ORIGINS_H_
