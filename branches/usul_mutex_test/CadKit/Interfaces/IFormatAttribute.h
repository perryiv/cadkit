
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IFormatAttribute: Interface for format attributes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_FORMAT_ATTRIBUTES_H_
#define _CADKIT_INTERFACE_FORMAT_ATTRIBUTES_H_

#include "Enum.h"
#include "IUnknown.h"

#include <string>


namespace CadKit
{
class IFormatAttribute : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1054759437 };

  // Does the format have the attribute?
  virtual bool            isAttributeSupported ( const FormatAttribute &attribute ) const = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_FORMAT_ATTRIBUTES_H_
