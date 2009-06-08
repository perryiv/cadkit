
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Update any needed data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IUPDATE_LISTENER_H__
#define __USUL_INTERFACES_IUPDATE_LISTENER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IUpdateListener : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IUpdateListener );

  /// Id for this interface.
  enum { IID = 4018508570u };

  // It's safe to update.
  virtual void updateNotify ( Usul::Interfaces::IUnknown *caller ) = 0;

}; // struct IUpdateListener


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IUPDATE_LISTENER_H__ */
