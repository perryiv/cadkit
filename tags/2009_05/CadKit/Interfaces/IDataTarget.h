
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

#include <string>


namespace CadKit
{
class IDataTarget : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032674768 };

  // Tell the target it is about to receive data.
  virtual bool            dataTransferStart ( IUnknown *caller ) = 0;

  // Tell the target it is done receiving data.
  virtual bool            dataTransferEnd ( IUnknown *caller ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_DATA_TARGET_H_
