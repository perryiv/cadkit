
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IOutputPrecision: Interface for output precisions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_OUTPUT_PRECISION_H_
#define _CADKIT_INTERFACE_OUTPUT_PRECISION_H_

#include "Enum.h"
#include "IUnknown.h"

#include <string>


namespace CadKit
{
class IOutputPrecision : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1055307022 };

  // Set the number of decimals to output.
  virtual bool            setOutputNumDecimals ( unsigned int numDecimals ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_OUTPUT_PRECISION_H_
