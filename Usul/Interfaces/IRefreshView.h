
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the view
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_REFRESH_VIEW_H_
#define _USUL_INTERFACE_REFRESH_VIEW_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {

struct IViewer;

struct IRefreshView : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRefreshView );

  /// Id for this interface.
  enum { IID = 4246412733u };

  virtual void refreshView ( Usul::Interfaces::IViewer* ) = 0;

}; // struct IRefreshView


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_REFRESH_VIEW_H_
