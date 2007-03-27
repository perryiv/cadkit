
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The wxWindows menu bar class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GUI_MENU_BAR_CLASS_H_
#define _WX_GUI_MENU_BAR_CLASS_H_

#include "AppFrameWork/Wx/CompileGuard.h"
#include "AppFrameWork/Menus/MenuBar.h"

namespace AFW { namespace Menus { class Button; } }
class wxMenuBar;
class wxCommandEvent;


class WxMenuBar : public AFW::Menus::MenuBar
{
public:

  // Typedefs.
  typedef AFW::Menus::MenuBar BaseClass;
  typedef BaseClass::Itr Itr;

  // Detach from gui object.
  virtual void                        detach();

  // Get the menu bar.
  const wxMenuBar *                   get() const;
  wxMenuBar *                         get();

  // Find the button corresponding to the event.
  AFW::Menus::Button *                findButton ( const wxCommandEvent & );

  // Build a default GUI.
  virtual void                        init();

  // Insert a window.
  virtual bool                        insert ( Itr where, AFW::Windows::Window * );

protected:

  // Default construction.
  WxMenuBar();

  // Use reference counting.
  virtual ~WxMenuBar();

private:

  // Do not copy.
  WxMenuBar ( const WxMenuBar & );
  WxMenuBar &operator = ( const WxMenuBar & );

  AFW_DECLARE_OBJECT ( WxMenuBar );
};


#endif // _WX_GUI_MENU_BAR_CLASS_H_
