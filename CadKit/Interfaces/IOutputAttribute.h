
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IOutputAttribute: Interface for output format attributes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_OUTPUT_ATTRIBUTE_H_
#define _CADKIT_INTERFACE_OUTPUT_ATTRIBUTE_H_

#include "Enum.h"
#include "IUnknown.h"

#include <string>


namespace CadKit
{
class IOutputAttribute : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1054759438 };

  // Set the output attribute.
  virtual bool            setOutputAttribute ( const FormatAttribute &attribute ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_OUTPUT_ATTRIBUTE_H_
