
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IDataWrite: Interface for writing data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_DATA_STORE_H_
#define _CADKIT_INTERFACE_DATA_STORE_H_

#include "IUnknown.h"

#include <string>


namespace CadKit
{
class IDataWrite : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1054759304 };

  // Write the data.
  virtual bool            writeData ( const std::string &name ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_DATA_STORE_H_
