
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for flushing messages from the queue.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_MESSAGE_QUEUE_FLUSH_H_
#define _USUL_INTERFACE_MESSAGE_QUEUE_FLUSH_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IMessageQueueFlush : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMessageQueueFlush );

  /// Id for this interface.
  enum { IID = 1685855450u };

  /// Flush messages.
  virtual bool            messageQueueFlushAll() = 0;
  virtual bool            messageQueueFlushOne() = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_MESSAGE_QUEUE_FLUSH_H_
