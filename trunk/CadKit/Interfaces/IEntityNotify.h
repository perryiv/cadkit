
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IEntityNotify: Interface for reporting the start/end of some entity.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_NOTIFY_H_
#define _CADKIT_INTERFACE_NOTIFY_H_

#include "IUnknown.h"
#include "Handles.h"


namespace CadKit
{
template <const unsigned int id, class EntityHandle> class IEntityNotify : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = id };

  // End the entity.
  virtual bool            endEntity ( EntityHandle entity, IUnknown *caller ) = 0;

  // Start the entity.
  virtual bool            startEntity ( EntityHandle entity, IUnknown *caller ) = 0;
};


// Common types.
typedef IEntityNotify<1032976180,AssemblyHandle>  IAssemblyNotify;
typedef IEntityNotify<1032976192,PartHandle>      IPartNotify;
typedef IEntityNotify<1032976199,InstanceHandle>  IInstanceNotify;
typedef IEntityNotify<1032980126,GroupHandle>     IGroupNotify;


}; // namespace CadKit

#endif // _CADKIT_INTERFACE_NOTIFY_H_
