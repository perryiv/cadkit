
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IShapeQuery: Interface for querying a shape. A shape is a collection of 
//  vertex sets that share the same material and texture.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_SHAPE_QUERY_H_
#define _CADKIT_INTERFACE_SHAPE_QUERY_H_

#include "IChildQuery.h"

#include "Standard/SlMaterial.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <vector>
#endif


namespace CadKit
{
template <const unsigned int id, class EntityHandle, class ParentHandle, class MaterialType, class TextureType> 
class IShapeQuery : public IChildQuery<id,EntityHandle,ParentHandle>
{
public:

  // Get the material.
  virtual bool            getMaterial ( EntityHandle assembly, SlMaterial<MaterialType> &material, bool tryParents ) const = 0;

  // Get the texture.
  virtual bool            getTexture ( EntityHandle assembly, std::vector<TextureType> &texture, bool tryParents ) const = 0;
};


// Common types.
typedef IShapeQuery<1033333106,ShapeHandle,LodHandle,float, unsigned char> IShapeQueryFloatUchar;
typedef IShapeQuery<1033333107,ShapeHandle,LodHandle,float, char>          IShapeQueryFloatChar;
typedef IShapeQuery<1033333108,ShapeHandle,LodHandle,double,unsigned char> IShapeQueryDoubleUchar;
typedef IShapeQuery<1033333109,ShapeHandle,LodHandle,double,char>          IShapeQueryDoubleChar;


}; // namespace CadKit


#endif // _CADKIT_INTERFACE_SHAPE_QUERY_H_
