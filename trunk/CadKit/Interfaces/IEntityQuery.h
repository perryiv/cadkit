
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

#include "IUnknown.h"
#include "Handles.h"

#include "Standard/SlMaterial.h"
#include "Standard/SlMatrix4.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
template <const unsigned int id, class EntityHandle, class Real> class IEntityQuery : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = id };

  // Get the name.
  virtual std::string     getName ( EntityHandle assembly ) const = 0;

  // Get the material.
  virtual bool            getMaterial ( EntityHandle assembly, SlMaterial<Real> &material ) const = 0;

  // Get the transformation matrix.
  virtual bool            getTransform ( EntityHandle assembly, SlMatrix4<Real> &matrix ) const = 0;
};


// Common types.
typedef IEntityQuery<1033021569,AssemblyHandle,float>  IAssemblyQueryFloat;
typedef IEntityQuery<1033021570,AssemblyHandle,double> IAssemblyQueryDouble;
typedef IEntityQuery<1033021571,PartHandle,    float>  IPartQueryFloat;
typedef IEntityQuery<1033021572,PartHandle,    double> IPartQueryDouble;
typedef IEntityQuery<1033021573,GroupHandle,   float>  IGroupQueryFloat;
typedef IEntityQuery<1033021574,GroupHandle,   double> IGroupQueryDouble;


}; // namespace CadKit


#endif // _CADKIT_INTERFACE_ENTITY_QUERY_H_
