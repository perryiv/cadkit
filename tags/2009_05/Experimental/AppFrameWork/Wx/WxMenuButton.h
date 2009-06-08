
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The wxWindows menu button class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GUI_MENU_BUTTON_CLASS_H_
#define _WX_GUI_MENU_BUTTON_CLASS_H_

#include "AppFrameWork/Wx/CompileGuard.h"
#include "AppFrameWork/Menus/Button.h"

class wxMenuItem;
class WxMenuGroup;


class WxMenuButton : public AFW::Menus::Button
{
public:

  // Typedefs.
  typedef AFW::Menus::Button BaseClass;

  // Set the checked state.
  virtual void                        check ( bool );

  // Create the button.
  wxMenuItem *                        create ( WxMenuGroup *menu );

  // Enable/disable the window.
  virtual void                        enable ( bool );

  // Detach from gui object.
  virtual void                        detach();

protected:

  // Default construction.
  WxMenuButton();

  // Use reference counting.
  virtual ~WxMenuButton();

private:

  // Do not copy.
  WxMenuButton ( const WxMenuButton & );
  WxMenuButton &operator = ( const WxMenuButton & );

  AFW_DECLARE_OBJECT ( WxMenuButton );
};


#endif // _WX_GUI_MENU_BUTTON_CLASS_H_
