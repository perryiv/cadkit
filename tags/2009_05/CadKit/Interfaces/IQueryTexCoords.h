
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IQueryTexCoords: Interface for querying texture coordinates.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_QUERY_TEXTURE_COORDINATES_H_
#define _CADKIT_INTERFACE_QUERY_TEXTURE_COORDINATES_H_

#include "IQueryArray.h"

#include "Standard/SlVec2.h"


namespace CadKit
{
template <const unsigned int id, class EntityHandle, class ArrayType> 
class IQueryTexCoords : public IQueryArray<id,ArrayType>
{
public:

  // Inherit from this and define the virtual functions.
  class TexCoordSetter : public IQueryArray<id,ArrayType>::ArraySetter{};

  // Get the texture coordinates.
  virtual bool            getTextureCoords ( EntityHandle entity, TexCoordSetter &setter ) const = 0;
};


// Common types.
typedef IQueryTexCoords<1033360403,ShapeHandle,SlVec2f> IQueryShapeTexCoordsVec2f;
typedef IQueryTexCoords<1033360404,ShapeHandle,SlVec2d> IQueryShapeTexCoordsVec2d;


}; // namespace CadKit


#endif // _CADKIT_INTERFACE_QUERY_TEXTURE_COORDINATES_H_
