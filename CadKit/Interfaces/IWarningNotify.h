
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IWarningNotify: Interface for reporting warnings.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_WARNING_NOTIFY_H_
#define _CADKIT_INTERFACE_WARNING_NOTIFY_H_

#include "IUnknown.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
class IWarningNotify : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032644947 };

  // Report a warning. Return true if the caller should continue.
  virtual bool            warningNotify ( const std::string &message, const unsigned long &id ) const = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_WARNING_NOTIFY_H_
