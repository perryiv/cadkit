
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IMessageNotify: Interface for reporting Messages.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_MESSAGE_NOTIFY_H_
#define _CADKIT_INTERFACE_MESSAGE_NOTIFY_H_

#include "IUnknown.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
class IMessageNotify : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1033067270 };

  // Message types.
  enum Type
  {
    MESSAGE_ERROR,
    MESSAGE_WARNING,
    MESSAGE_QUESTION,
    MESSAGE_INFO,
    MESSAGE_PROGRESS
  };

  // Notification of a message.
  virtual bool            messageNotify ( const std::string &message, const unsigned long &id, const Type &type ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_MESSAGE_NOTIFY_H_
