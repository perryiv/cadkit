
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IPrimQuery: Interface for querying a primitive. A primitive is a 
//  simple object.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_PRIM_QUERY_H_
#define _CADKIT_INTERFACE_PRIM_QUERY_H_

#include "IChildQuery.h"

#include "Standard/SlMaterial.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <vector>
#endif


namespace CadKit
{
template <const unsigned int id, class EntityHandle, class ParentHandle, class MaterialType> 
class IPrimQuery : public IChildQuery<id,EntityHandle,ParentHandle>
{
public:

  // Get the material.
  virtual bool            getMaterial ( EntityHandle assembly, SlMaterial<MaterialType> &material, bool tryParents ) const = 0;
};


// Common types.
typedef IPrimQuery<1033333111,PrimHandle,LodHandle,float>   IPrimQueryFloat;
typedef IPrimQuery<1033333112,PrimHandle,LodHandle,double>  IPrimQueryDouble;

}; // namespace CadKit


#endif // _CADKIT_INTERFACE_PRIM_QUERY_H_
