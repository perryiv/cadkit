
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The wxWindows menu group class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GUI_MENU_GROUP_CLASS_H_
#define _WX_GUI_MENU_GROUP_CLASS_H_

#include "AppFrameWork/Wx/CompileGuard.h"
#include "AppFrameWork/Menus/MenuGroup.h"

class wxMenu;


class WxMenuGroup : public AFW::Menus::MenuGroup
{
public:

  // Typedefs.
  typedef AFW::Menus::MenuGroup BaseClass;
  typedef BaseClass::Itr Itr;

  // Detach from gui object.
  virtual void                        detach();

  // Get the menu.
  const wxMenu *                      get() const;
  wxMenu *                            get();

  // Insert a window.
  virtual bool                        insert ( Itr where, AFW::Windows::Window * );

protected:

  // Default construction.
  WxMenuGroup();

  // Use reference counting.
  virtual ~WxMenuGroup();

private:

  // Do not copy.
  WxMenuGroup ( const WxMenuGroup & );
  WxMenuGroup &operator = ( const WxMenuGroup & );

  AFW_DECLARE_OBJECT ( WxMenuGroup );
};


#endif // _WX_GUI_MENU_GROUP_CLASS_H_
