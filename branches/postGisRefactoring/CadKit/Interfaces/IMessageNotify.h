
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IMessageNotify: Interface for reporting messages.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_MESSAGE_NOTIFY_H_
#define _CADKIT_INTERFACE_MESSAGE_NOTIFY_H_

#include "Enum.h"
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

  // Notification of a message.
  virtual bool            messageNotify ( const std::string &message, unsigned long id, const MessageType &type ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_MESSAGE_NOTIFY_H_
