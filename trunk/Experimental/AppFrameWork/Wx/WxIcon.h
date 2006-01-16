
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Icon class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GUI_ICON_CLASS_H_
#define _WX_GUI_ICON_CLASS_H_

#include "AppFrameWork/Wx/CompileGuard.h"

namespace AFW { namespace Core { class Icon; } }
namespace AFW { namespace Windows { class Window; } }


struct WxIcon
{
  // Load the icon. We return a copy because wxIcon uses reference counting 
  // for its internal data.
  // See: http://www.wxwindows.org/manuals/2.6.2/wx_wxicon.html
  static wxIcon          load ( const AFW::Windows::Window *window, bool reportFailure = true );
  static wxIcon          load ( const AFW::Core::Icon &icon, bool reportFailure = true );
  static wxIcon          load ( const std::string &file, bool reportFailure = true );
};


#endif // _WX_GUI_ICON_CLASS_H_
