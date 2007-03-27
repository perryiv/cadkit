
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IEntityQuery: Interface for querying an entity.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_ENTITY_QUERY_H_
#define _CADKIT_INTERFACE_ENTITY_QUERY_H_

#include "IChildQuery.h"

#include "Standard/SlMaterial.h"
#include "Standard/SlMatrix44.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
template <const unsigned int id, class EntityHandle, class ParentHandle, class Real> 
class IEntityQuery : public IChildQuery<id,EntityHandle,ParentHandle>
{
public:

  // Get the name.
  virtual std::string     getName ( EntityHandle entity ) const = 0;

  // Get the material.
  virtual bool            getMaterial ( EntityHandle entity, SlMaterial<Real> &material, bool tryParents ) const = 0;

  // Get the transformation matrix.
  virtual bool            getTransform ( EntityHandle entity, SlMatrix44<Real> &matrix, bool tryParents ) const = 0;
};


// Common types.
typedef IEntityQuery<1033021569,AssemblyHandle,AssemblyHandle,float>  IAssemblyQueryFloat;
typedef IEntityQuery<1033021570,AssemblyHandle,AssemblyHandle,double> IAssemblyQueryDouble;
typedef IEntityQuery<1033021571,PartHandle,    AssemblyHandle,float>  IPartQueryFloat;
typedef IEntityQuery<1033021572,PartHandle,    AssemblyHandle,double> IPartQueryDouble;
typedef IEntityQuery<1033021573,GroupHandle,   GroupHandle,   float>  IGroupQueryFloat;
typedef IEntityQuery<1033021574,GroupHandle,   GroupHandle,   double> IGroupQueryDouble;


}; // namespace CadKit


#endif // _CADKIT_INTERFACE_ENTITY_QUERY_H_
