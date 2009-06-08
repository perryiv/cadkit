
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IOutputUnits: Interface for setting linear units.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_OUTPUT_UNITS_H_
#define _CADKIT_INTERFACE_OUTPUT_UNITS_H_

#include "IUnknown.h"

#include <string>


namespace CadKit
{
class IOutputUnits : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1064385609 };

  // Get the units.
  virtual void setUnits( const std::string &units ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_OUTPUT_UNITS_H_
