
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IPartNotify: Interface for reporting an part.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_PART_NOTIFY_H_
#define _CADKIT_INTERFACE_PART_NOTIFY_H_

#include "IUnknown.h"
#include "Handles.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
class IPartNotify : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032919163 };

  // End the Part.
  virtual bool            endPart ( PartHandle Part, IUnknown *caller ) = 0;

  // Start the Part.
  virtual bool            startPart ( PartHandle Part, IUnknown *caller ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_PART_NOTIFY_H_
