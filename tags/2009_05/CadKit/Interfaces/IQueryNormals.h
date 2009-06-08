
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IQueryNormals: Interface for querying normals.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_QUERY_NORMALS_H_
#define _CADKIT_INTERFACE_QUERY_NORMALS_H_

#include "IQueryArray.h"
#include "Enum.h"

#include "Standard/SlVec3.h"


namespace CadKit
{
template <const unsigned int id, class EntityHandle, class ArrayType> 
class IQueryNormals : public IQueryArray<id,ArrayType>
{
public:

  // Inherit from this and define the virtual functions.
  class NormalSetter : public IQueryArray<id,ArrayType>::ArraySetter{};

  // Get the normal binding.
  virtual bool            getNormalBinding ( EntityHandle entity, VertexBinding &binding ) const = 0;

  // Get the normals.
  virtual bool            getNormals ( EntityHandle entity, NormalSetter &setter ) const = 0;
};


// Common types.
typedef IQueryNormals<1033359881,ShapeHandle,SlVec3f> IQueryShapeNormalsVec3f;
typedef IQueryNormals<1033359882,ShapeHandle,SlVec3d> IQueryShapeNormalsVec3d;


}; // namespace CadKit


#endif // _CADKIT_INTERFACE_QUERY_NORMALS_H_
