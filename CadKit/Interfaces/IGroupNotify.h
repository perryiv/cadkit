
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IGroupNotify: Interface for reporting a group.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_GROUP_NOTIFY_H_
#define _CADKIT_INTERFACE_GROUP_NOTIFY_H_

#include "IUnknown.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
class IGroupNotify : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032725316 };

  // End the assembly.
  virtual bool            startGroup ( IUnknown *caller ) = 0;

  // Start the assembly.
  virtual bool            endGroup ( IUnknown *caller ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_GROUP_NOTIFY_H_
