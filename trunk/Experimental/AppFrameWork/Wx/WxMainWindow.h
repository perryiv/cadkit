
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
#include "AppFrameWork/Windows/MainWindow.h"

class WxMenuBar;
class WxStatusBar;


class WxMainWindow : public AFW::Windows::MainWindow
{
public:

  // Typedefs.
  typedef AFW::Windows::MainWindow BaseClass;
  typedef BaseClass::Itr Itr;
  typedef AFW::Windows::Window::Position Position;
  typedef AFW::Windows::Window::Size Size;

  // Write configuration to disk.
  virtual void                        configWrite() const;

  // Detach from gui object.
  virtual void                        detach();

  // Insert a window.
  virtual bool                        insert ( Itr where, AFW::Windows::Window * );

  // Set/get the frame's maximized state
  virtual void                        maximized ( bool );
  virtual bool                        maximized() const;

  // Get/set the menu bar.
  const WxMenuBar *                   menuBar() const;
  WxMenuBar *                         menuBar();
  virtual void                        menuBar ( AFW::Menus::MenuBar * );

  // Set/get the position.
  virtual void                        position ( const Position & );
  virtual Position                    position() const;

  // Set/get window size.
  virtual void                        size ( const Size & );
  virtual Size                        size() const;

  // Set/get the status bar.
  const WxStatusBar *                 statusBar() const;
  WxStatusBar *                       statusBar();
  virtual void                        statusBar ( AFW::Windows::StatusBar * );

  // Hide/show the child.
  virtual void                        visible ( AFW::Windows::Window *, bool );

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
