
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IInstanceNotify: Interface for reporting an instance.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_INSTANCE_NOTIFY_H_
#define _CADKIT_INTERFACE_INSTANCE_NOTIFY_H_

#include "IUnknown.h"
#include "Handles.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
class IInstanceNotify : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032919224 };

  // End the Instance.
  virtual bool            endInstance ( InstanceHandle Instance, IUnknown *caller ) = 0;

  // Start the Instance.
  virtual bool            startInstance ( InstanceHandle Instance, IUnknown *caller ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_INSTANCE_NOTIFY_H_
