
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

#ifndef _CADKIT_INTERFACE_ENTITY_NOTIFY_H_
#define _CADKIT_INTERFACE_ENTITY_NOTIFY_H_

#include "IUnknown.h"
#include "Handles.h"


namespace CadKit
{
template <const unsigned int id, class EntityHandle> class IEntityNotify : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = id };

  // End the entity. When this function returns, the handle may no be longer valid.
  virtual bool            endEntity ( EntityHandle entity, IUnknown *caller ) = 0;

  // Start the entity. This handle is valid until endEntity() is called.
  virtual bool            startEntity ( EntityHandle entity, IUnknown *caller ) = 0;
};


// Common types.
typedef IEntityNotify<1032976180,AssemblyHandle>      IAssemblyNotify;
typedef IEntityNotify<1032976192,PartHandle>          IPartNotify;
typedef IEntityNotify<1032976199,InstanceHandle>      IInstanceNotify;
typedef IEntityNotify<1032980126,GroupHandle>         IGroupNotify;
typedef IEntityNotify<1033138075,LodHandle>           ILodNotify;
typedef IEntityNotify<1033270378,ShapeHandle>         IShapeNotify;
typedef IEntityNotify<1033270379,PrimHandle>          IPrimNotify;
typedef IEntityNotify<1033272557,SetHandle>           ISetNotify;


}; // namespace CadKit

#endif // _CADKIT_INTERFACE_ENTITY_NOTIFY_H_
