
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IErrorNotify: Interface for reporting errors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_ERROR_NOTIFY_H_
#define _CADKIT_INTERFACE_ERROR_NOTIFY_H_

#include "IUnknown.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
class IErrorNotify : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032561464 };

  // Report an error. Return true if the caller should continue.
  virtual bool            errorNotify ( const std::string &message, const unsigned long &id ) const = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_ERROR_NOTIFY_H_
