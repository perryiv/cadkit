

///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Get and set pointer to the status bar.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Resources/StatusBar.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Pointer to the status bar.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Resources
  {
    namespace Detail
    {
      Usul::Interfaces::IStatusBar::QueryPtr _statusBar;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get pointer to the status bar. It may be null.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IStatusBar *Usul::Resources::statusBar()
{
  return Detail::_statusBar.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set pointer to the status bar.
//
///////////////////////////////////////////////////////////////////////////////

void Usul::Resources::statusBar ( Usul::Interfaces::IStatusBar *statusBar )
{
  Detail::_statusBar = statusBar;
} 

