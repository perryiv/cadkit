
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for posting a message to the queue.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_MESSAGE_QUEUE_POST_H_
#define _USUL_INTERFACE_MESSAGE_QUEUE_POST_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


template
<
  class MessageType, 
  unsigned long iid
>
struct IMessageQueuePost : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMessageQueuePost );

  /// Id for this interface.
  enum { IID = iid };

  /// Post the message.
  virtual bool            messageQueuePost ( MessageType message ) = 0;
};


typedef IMessageQueuePost < unsigned short, 2092838157u > IMessageQueuePostUShort;
typedef IMessageQueuePost < unsigned long,  3130254695u > IMessageQueuePostULong;
typedef IMessageQueuePost < unsigned int,   1794573985u > IMessageQueuePostUInt;


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_MESSAGE_QUEUE_POST_H_
