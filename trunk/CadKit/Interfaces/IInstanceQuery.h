
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

#include "IUnknown.h"
#include "Handles.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
template <const unsigned int id, class Real> class IInstanceQuery : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = id };

  // Get the corresponding part.
  virtual PartHandle      getCorrespondingPart ( InstanceHandle instance ) const = 0;

  // Get the name.
  virtual std::string     getName ( InstanceHandle instance ) const = 0;

  // Get the transformation matrix.
  virtual bool            getTransform ( InstanceHandle instance, Real matrix[16] ) const = 0;
};


// Common types.
typedef IInstanceQuery<1032919857,float>  IInstanceQueryFloat;
typedef IInstanceQuery<1032919869,double> IInstanceQueryDouble;


}; // namespace CadKit

#endif // _CADKIT_INTERFACE_INSTANCE_QUERY_H_
