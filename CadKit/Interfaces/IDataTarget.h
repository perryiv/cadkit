
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IDataTarget: Interface for a data target.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_DATA_TARGET_H_
#define _CADKIT_INTERFACE_DATA_TARGET_H_

#include "IUnknown.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
class IDataTarget : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032674768 };

  // Get the file extension.
  virtual std::string     getFileExtension() const = 0;

  // Get the default output name, based on the given source name.
  virtual std::string     getDefaultOutputName ( const std::string &inputName ) = 0;

  // Store the data.
  virtual bool            storeData ( const std::string &name ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_DATA_TARGET_H_
