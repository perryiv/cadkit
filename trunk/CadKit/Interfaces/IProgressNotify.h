
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IProgressNotify: Interface for reporting progress.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_PROGRESS_NOTIFY_H_
#define _CADKIT_INTERFACE_PROGRESS_NOTIFY_H_

#include "IUnknown.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
class IProgressNotify : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032645618 };

  // Report progress. Return true if the caller should continue.
  virtual bool            progressNotify ( const std::string &message ) const = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_WARNING_H_
