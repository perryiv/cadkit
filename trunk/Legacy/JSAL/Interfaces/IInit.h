
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Init-notify interface.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _JSAL_INTERFACE_NOTIFY_INIT_H_
#define _JSAL_INTERFACE_NOTIFY_INIT_H_

#include "Usul/Interfaces/IUnknown.h"


namespace JSAL {
namespace Interfaces {


struct IInit : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IInit );

  /// Id for this interface.
  enum { IID = 2135801058 };

  // Tell the component that the program is initializing.
  virtual void              init() = 0;
};


}; // namespace Interfaces
}; // namespace JSAL


#endif // _JSAL_INTERFACE_NOTIFY_INIT_H_
