
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IAssemblyNotify: Interface for reporting an assembly.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_ASSEMBLY_NOTIFY_H_
#define _CADKIT_INTERFACE_ASSEMBLY_NOTIFY_H_

#include "IUnknown.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
class IAssemblyNotify : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032724907 };

  // End the assembly.
  virtual bool            endAssembly ( IUnknown *caller ) = 0;

  // Start the assembly.
  virtual bool            startAssembly ( IUnknown *caller ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_ASSEMBLY_NOTIFY_H_
