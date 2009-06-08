
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

#ifndef _CADKIT_INTERFACE_QUERY_PRIM_PARAMS_H_
#define _CADKIT_INTERFACE_QUERY_PRIM_PARAMS_H_

#include "IQueryArray.h"
#include "Enum.h"


namespace CadKit
{
template <const unsigned int id, class EntityHandle, class ArrayType> 
class IQueryPrimParams : public IQueryArray<id,ArrayType>
{
public:

  // Inherit from this and define the virtual functions.
  class ParamSetter : public IQueryArray<id,ArrayType>::ArraySetter {};

  // Get the set type.
  virtual bool            getPrimType ( EntityHandle entity, PrimitiveType &type ) const = 0;

  // Get the vertices.
  virtual bool            getPrimParams ( EntityHandle entity, ParamSetter &setter ) const = 0;
};


// Common types.
typedef IQueryPrimParams<1033359888,PrimHandle,float> IQueryPrimParamsFloat;
typedef IQueryPrimParams<1033359889,PrimHandle,double> IQueryPrimParamsDouble;


}; // namespace CadKit


#endif // _CADKIT_INTERFACE_QUERY_PRIM_PARAMS_H_
