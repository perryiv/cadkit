
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IInstanceQuery: Interface for querying an instance.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_INSTANCE_QUERY_H_
#define _CADKIT_INTERFACE_INSTANCE_QUERY_H_

#include "IEntityQuery.h"


namespace CadKit
{
template <const unsigned int id, class Real> 
class IInstanceQuery : public IEntityQuery<id,InstanceHandle,AssemblyHandle,Real>
{
public:

  // Get the assembly that the given instance represents, or null.
  virtual AssemblyHandle  getCorrespondingAssembly ( InstanceHandle instance ) const = 0;

  // Get the part that the given instance represents, or null.
  virtual PartHandle      getCorrespondingPart ( InstanceHandle instance ) const = 0;
};


// Common types.
typedef IInstanceQuery<1033022176,float>  IInstanceQueryFloat;
typedef IInstanceQuery<1033022177,double> IInstanceQueryDouble;


}; // namespace CadKit

#endif // _CADKIT_INTERFACE_INSTANCE_QUERY_H_
