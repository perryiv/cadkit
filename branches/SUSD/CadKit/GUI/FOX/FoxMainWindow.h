
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Main window for the GUI-FOX library.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_GUI_FOX_MAIN_WINDOW_H_
#define _CADKIT_GUI_FOX_MAIN_WINDOW_H_

#ifdef _WIN32
# define NOMINMAX // Fox includes <windows.h>
#endif

#include <fx.h>

#include "FoxApi.h"

#include "Standard/SlAutoPtr.h"


namespace CadKit
{
class GUI_FOX_API FoxMainWindow : public FXMainWindow 
{
public:

  // Message ids.
  enum 
  {
    ID_ABOUT = FXMainWindow::ID_LAST,
    ID_LAST
  };

  // Need to call after init if GUI is already up and running.
  virtual void        create();

  // Is it created?
  bool                isCreated() const { return ( 0x0 != this->id() ); }

  // Two-step construction.
  virtual bool        init();

  // Message handlers.
  long                onCommandAbout ( FXObject *, FXSelector, void * );

protected:

  SlAutoPtr<FXMenuBar> _menuBar;
  SlAutoPtr<FXStatusBar> _statusBar;
  SlAutoPtr<FXMenuPane> _fileMenu;
  SlAutoPtr<FXMenuPane> _windowMenu;
  SlAutoPtr<FXMenuPane> _helpMenu;

  FoxMainWindow ( 
    FXApp *app,
    const FXString &name,
    FXIcon *icon = NULL,
    FXIcon *mi = NULL,
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
  FoxMainWindow();
  virtual ~FoxMainWindow();

  FXDECLARE ( FoxMainWindow );
};


}; // namespace CadKit


#endif _CADKIT_GUI_FOX_MAIN_WINDOW_H_
