
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IMessagePriority: Interface for setting the message priority level.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_MESSAGE_PRIORITY_H_
#define _CADKIT_INTERFACE_MESSAGE_PRIORITY_H_

#include "Enum.h"
#include "IUnknown.h"


namespace CadKit
{
class IMessagePriority : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1034027225 };

  // Set the message priority level.
  virtual bool            setMessagePriorityLevel ( const MessageType &type, unsigned int priority ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_MESSAGE_PRIORITY_H_
