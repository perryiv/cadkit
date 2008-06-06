
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IInterfaceGroup: A collection of IUnknown interfaces.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_INTERFACE_GROUP_H_
#define _CADKIT_INTERFACE_INTERFACE_GROUP_H_

#include "IUnknown.h"


namespace CadKit
{
class IInterfaceGroup : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032636764 };

  // Add the interface.
  virtual void            addInterface ( IUnknown *unknown ) = 0;

  // Get the number of unknowns.
  virtual unsigned int    getNumInterfaces() const = 0;

  // Remove the interface. Returns the number of occurances removed.
  virtual unsigned int    removeInterface ( IUnknown *unknown ) = 0;

  // Remove all interfaces.
  virtual void            removeAllInterfaces() = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_INTERFACE_GROUP_H_
