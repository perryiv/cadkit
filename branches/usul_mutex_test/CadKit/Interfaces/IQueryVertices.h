
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

#ifndef _CADKIT_INTERFACE_QUERY_VERTICES_H_
#define _CADKIT_INTERFACE_QUERY_VERTICES_H_

#include "IQueryArray.h"
#include "Enum.h"

#include "Standard/SlVec3.h"


namespace CadKit
{
template <const unsigned int id, class EntityHandle, class ArrayType> 
class IQueryVertices : public IQueryArray<id,ArrayType>
{
public:

  // Inherit from this and define the virtual functions.
  class VertexSetter : public IQueryArray<id,ArrayType>::ArraySetter
  {
  public:
    virtual bool setNumPrimitives  ( const unsigned int &num ) = 0;
    virtual bool setPrimitiveRange ( const unsigned int &index, const unsigned int &start, const unsigned int &length ) = 0;
  };

  // Get the set type.
  virtual bool            getVertexSetType ( EntityHandle entity, VertexSetType &type ) const = 0;

  // Get the vertices.
  virtual bool            getVertices ( EntityHandle entity, VertexSetter &setter ) const = 0;
};


// Common types.
typedef IQueryVertices<1033359883,ShapeHandle,SlVec3f> IQueryShapeVerticesVec3f;
typedef IQueryVertices<1033359884,ShapeHandle,SlVec3d> IQueryShapeVerticesVec3d;


}; // namespace CadKit


#endif // _CADKIT_INTERFACE_QUERY_VERTICES_H_
