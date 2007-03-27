
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the active view
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_ACTIVE_VIEW_H_
#define _USUL_INTERFACE_ACTIVE_VIEW_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IActiveView : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IActiveView );

  /// Id for this interface.
  enum { IID = 1101087805u };

  virtual Usul::Interfaces::IUnknown* getActiveView() = 0;
}; // class IActiveView


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_ACTIVE_VIEW_H_
