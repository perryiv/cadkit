
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  ISubEntityNotify: Interface for reporting the start/end of a sub-entity.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_SUB_ENTITY_NOTIFY_H_
#define _CADKIT_INTERFACE_SUB_ENTITY_NOTIFY_H_

#include "IUnknown.h"
#include "Handles.h"


namespace CadKit
{
template <const unsigned int id, class EntityHandle> class ISubEntityNotify : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = id };

  // End the sub-entity.
  virtual bool            endSubEntity ( const unsigned int &which, EntityHandle entity, IUnknown *caller ) = 0;

  // Start the sub-entity.
  virtual bool            startSubEntity ( const unsigned int &which, EntityHandle entity, IUnknown *caller ) = 0;
};


// Common types.
typedef ISubEntityNotify<1033245740,PartHandle>  IPartLodNotify;
typedef ISubEntityNotify<1033245748,GroupHandle> IGroupLodNotify;


}; // namespace CadKit

#endif // _CADKIT_INTERFACE_SUB_ENTITY_NOTIFY_H_
