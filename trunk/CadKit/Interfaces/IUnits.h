
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IUnits: Interface for linear units.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_UNITS_H_
#define _CADKIT_INTERFACE_UNITS_H_

#include "IUnknown.h"

#include <string>


namespace CadKit
{
class IUnits : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1064385608 };

  // Get the units.
  virtual std::string     getUnits() const = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_UNITS_H_
