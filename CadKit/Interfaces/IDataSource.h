
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IDataSource: Interface for a data source.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_DATA_SOURCE_H_
#define _CADKIT_INTERFACE_DATA_SOURCE_H_

#include "IUnknown.h"

#include <string>


namespace CadKit
{
class IDataSource : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032633421 };

  // Set the data target.
  virtual void            setDataTarget ( IUnknown *target ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_DATA_SOURCE_H_
