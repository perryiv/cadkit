
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for handling a message
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_HANDLE_MESSAGE_H_
#define _USUL_INTERFACE_HANDLE_MESSAGE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IHandleMessage : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IHandleMessage );

  /// Id for this interface.
  enum { IID = 1410321462u };

  virtual void handleMessage ( unsigned short message ) = 0;

}; // struct IHandleMessage


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_HANDLE_MESSAGE_H_
