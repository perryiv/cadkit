
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IDataRead: Interface for reading data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_DATA_READ_H_
#define _CADKIT_INTERFACE_DATA_READ_H_

#include "IUnknown.h"

#include <string>


namespace CadKit
{
class IDataRead : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1054759303 };

  // Read the data.
  virtual bool            readData ( const std::string &name ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_DATA_READ_H_
