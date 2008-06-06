
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IDOCK_WIDGET_MENU_H__
#define __USUL_INTERFACES_IDOCK_WIDGET_MENU_H__

#include "Usul/Interfaces/IUnknown.h"

class QDockWidget;

namespace Usul {
namespace Interfaces {


struct IQtDockWidgetMenu : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IQtDockWidgetMenu );

  /// Id for this interface.
  enum { IID = 1929660250u };

  virtual void                     addDockWidgetMenu ( QDockWidget * ) = 0;

}; // struct IQtDockWidgetMenu


} // End namespace Interfaces
} // End namespace Usul

#endif // __USUL_INTERFACES_IDOCK_WIDGET_MENU_H__
