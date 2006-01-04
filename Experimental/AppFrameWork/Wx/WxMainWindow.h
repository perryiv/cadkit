
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The wxWindows main window class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GUI_MAIN_WINDOW_CLASS_H_
#define _WX_GUI_MAIN_WINDOW_CLASS_H_

#include "AppFrameWork/Wx/CompileGuard.h"
#include "AppFrameWork/Core/MainWindow.h"

class WxMenuBar;
class WxStatusBar;


class WxMainWindow : public AFW::Core::MainWindow
{
public:

  // Typedefs.
  typedef AFW::Core::MainWindow BaseClass;
  typedef BaseClass::Itr Itr;
  typedef AFW::Core::Window::Position Position;
  typedef AFW::Core::Window::Size Size;

  // Write configuration to disk.
  virtual void                        configWrite() const;

  // Detach from gui object.
  virtual void                        detach();

  // Insert a window.
  virtual bool                        insert ( Itr where, AFW::Core::Window * );

  // Get/set the menu bar.
  const WxMenuBar *                   menuBar() const;
  WxMenuBar *                         menuBar();
  virtual void                        menuBar ( AFW::Menus::MenuBar * );

  // Return the position.
  virtual Position                    position() const;

  // Return window size.
  virtual Size                        size() const;

  // Set/get the status bar.
  const WxStatusBar *                 statusBar() const;
  WxStatusBar *                       statusBar();
  virtual void                        statusBar ( AFW::Core::StatusBar * );

protected:

  // Default construction.
  WxMainWindow();

  // Use reference counting.
  virtual ~WxMainWindow();

  void                                _configRead ( wxPoint &origin, wxSize &size, bool &maximized );

private:

  // Do not copy.
  WxMainWindow ( const WxMainWindow & );
  WxMainWindow &operator = ( const WxMainWindow & );

  wxInterfaceManager *_ifm;

  AFW_DECLARE_OBJECT ( WxMainWindow );
};


#endif // _WX_GUI_MAIN_WINDOW_CLASS_H_
