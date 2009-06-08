
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IClientData: Interface for setting and getting client data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_CLIENT_DATA_H_
#define _CADKIT_INTERFACE_CLIENT_DATA_H_

#include "IUnknown.h"
#include "Handles.h"


namespace CadKit
{
template <const unsigned int id, class EntityHandle> class IClientData : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = id };

  // Get the client data.
  virtual const void *    getClientData ( const EntityHandle key ) const = 0;

  // Set the client data.
  virtual void            setClientData ( const EntityHandle key, const void *data ) = 0;
};


// Common types.
typedef IClientData<1033250439,void *>         IGenericClientData;
typedef IClientData<1033250440,AssemblyHandle> IAssemblyClientData;
typedef IClientData<1033250441,PartHandle>     IPartClientData;
typedef IClientData<1033250442,InstanceHandle> IInstanceClientData;
typedef IClientData<1033250443,GroupHandle>    IGroupClientData;
typedef IClientData<1033250444,LodHandle>      ILodClientData;
typedef IClientData<1033250445,ShapeHandle>    IShapeClientData;


}; // namespace CadKit

#endif // _CADKIT_INTERFACE_CLIENT_DATA_H_
