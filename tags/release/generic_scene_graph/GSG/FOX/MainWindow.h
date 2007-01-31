
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Main window.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_FOX_BINDING_MAIN_WINDOW_H_
#define _GENERIC_SCENE_GRAPH_FOX_BINDING_MAIN_WINDOW_H_

#include "GSG/Core/Config.h"

#include "GSG/FOX/FoxDll.h"

#include "fx.h"

#include <memory>


namespace GSG {
namespace FOX {


class GSG_FOX_EXPORT MainWindow : public FXMainWindow 
{
public:

  // Message ids.
  enum 
  {
    ID_ABOUT = FXMainWindow::ID_LAST,
    ID_LAST
  };

  // Is it created?
  bool                isCreated() const;

  // Message handlers.
  long                onCommandAbout ( FXObject *, FXSelector, void * );

protected:

  MainWindow ( 
    FXApp *app,
    const FXString &name,
    FXIcon *icon = 0x0,
    FXIcon *mi = 0x0,
    FXuint options = DECOR_ALL,
    FXint x = 0,
    FXint y = 0,
    FXint w = 0,
    FXint h = 0,
    FXint pl = 0,
    FXint pr = 0,
    FXint pt = 0,
    FXint pb = 0,
    FXint hs = 0,
    FXint vs = 0 );
  MainWindow();
  virtual ~MainWindow();

  // Get the status bar
  const FXStatusBar * statusBar() const { return _statusBar; }
  FXStatusBar *       statusBar()       { return _statusBar; }

  // Get the window pane.
  const FXMenuPane *  windowMenu() const { return _windowMenu; }
  FXMenuPane *        windowMenu()       { return _windowMenu; }

  // Get the menu bar.
  const FXMenuBar *   menuBar() const { return _menuBar; }
  FXMenuBar *         menuBar()       { return _menuBar; }

  // Get the file menu.
  const FXMenuPane *  fileMenu() const { return _fileMenu; }
  FXMenuPane *        fileMenu()       { return _fileMenu; }

  // Get the help menu.
  const FXMenuPane *  helpMenu() const { return _helpMenu; }
  FXMenuPane *        helpMenu()       { return _helpMenu; }

private:

  typedef Pointer < FXMenuBar,   NullOkPolicy, std::auto_ptr < FXMenuBar   > > FXMenuBarPtr;
  typedef Pointer < FXStatusBar, NullOkPolicy, std::auto_ptr < FXStatusBar > > FXStatusBarPtr;
  typedef Pointer < FXMenuPane,  NullOkPolicy, std::auto_ptr < FXMenuPane  > > FXMenuPanePtr;

  FXMenuBarPtr   _menuBar;
  FXStatusBarPtr _statusBar;
  FXMenuPanePtr  _fileMenu;
  FXMenuPanePtr  _windowMenu;
  FXMenuPanePtr  _helpMenu;

  FXDECLARE ( MainWindow );
};


}; // namespace FOX
}; // namespace GSG


#endif _GENERIC_SCENE_GRAPH_FOX_BINDING_MAIN_WINDOW_H_
