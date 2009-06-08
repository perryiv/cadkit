
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Context-specific initialization notify interface.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _JSAL_INTERFACE_NOTIFY_CONTEXT_INIT_H_
#define _JSAL_INTERFACE_NOTIFY_CONTEXT_INIT_H_

#include "Usul/Interfaces/IUnknown.h"


namespace JSAL {
namespace Interfaces {


struct IContextInit : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IContextInit );

  /// Id for this interface.
  enum { IID = 4124251387u };

  // Tell the component that the context is initializing.
  virtual void              contextInit() = 0;
};


}; // namespace Interfaces
}; // namespace JSAL


#endif // _JSAL_INTERFACE_NOTIFY_CONTEXT_INIT_H_
