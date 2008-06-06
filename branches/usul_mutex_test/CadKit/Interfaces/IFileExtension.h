
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IFileExtension: Interface for the file extension.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_FILE_EXTENSION_H_
#define _CADKIT_INTERFACE_FILE_EXTENSION_H_

#include "Enum.h"
#include "IUnknown.h"

#include <string>


namespace CadKit
{
class IFileExtension : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1054759436 };

  // Get the file extension.
  virtual std::string     getFileExtension() const = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_FILE_EXTENSION_H_
