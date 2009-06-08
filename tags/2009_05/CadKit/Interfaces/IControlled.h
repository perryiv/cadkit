
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IControlled: Interface for an object that is controlled by another.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_CONTROLLED_H_
#define _CADKIT_INTERFACE_CONTROLLED_H_

#include "IUnknown.h"


namespace CadKit
{
class IControlled : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032634450 };

  // Set the controller.
  virtual void            setController ( IUnknown *controller ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_CONTROLLED_H_
