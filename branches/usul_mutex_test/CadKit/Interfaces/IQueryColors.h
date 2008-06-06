
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IQueryColors: Interface for querying colors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_QUERY_COLORS_H_
#define _CADKIT_INTERFACE_QUERY_COLORS_H_

#include "IQueryArray.h"
#include "Enum.h"

#include "Standard/SlVec3.h"
#include "Standard/SlVec4.h"


namespace CadKit
{
template <const unsigned int id, class EntityHandle, class ArrayType> 
class IQueryColors : public IQueryArray<id,ArrayType>
{
public:

  // Inherit from this and define the virtual functions.
  class ColorSetter : public IQueryArray<id,ArrayType>::ArraySetter{};

  // Get the color binding.
  virtual bool            getColorBinding ( EntityHandle entity, VertexBinding &binding ) const = 0;

  // Get the colors.
  virtual bool            getColors ( EntityHandle entity, ColorSetter &setter ) const = 0;
};


// Common types.
typedef IQueryColors<1033359991,ShapeHandle,SlVec3f> IQueryShapeColorsVec3f;
typedef IQueryColors<1033359992,ShapeHandle,SlVec3d> IQueryShapeColorsVec3d;
typedef IQueryColors<1033359993,ShapeHandle,SlVec4f> IQueryShapeColorsVec4f;
typedef IQueryColors<1033359994,ShapeHandle,SlVec4d> IQueryShapeColorsVec4d;

typedef IQueryColors<1033359995,PrimHandle,SlVec3f> IQueryPrimColorsVec3f;
typedef IQueryColors<1033359996,PrimHandle,SlVec3d> IQueryPrimColorsVec3d;
typedef IQueryColors<1033359997,PrimHandle,SlVec4f> IQueryPrimColorsVec4f;
typedef IQueryColors<1033359998,PrimHandle,SlVec4d> IQueryPrimColorsVec4d;


}; // namespace CadKit


#endif // _CADKIT_INTERFACE_QUERY_COLORS_H_
