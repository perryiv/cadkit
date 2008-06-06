
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

#ifndef _CADKIT_GUI_FOX_MAIN_MDI_WINDOW_H_
#define _CADKIT_GUI_FOX_MAIN_MDI_WINDOW_H_

#include "FoxMainWindow.h"


namespace CadKit
{
class GUI_FOX_API FoxMdiMainWindow : public FoxMainWindow 
{
public:

  // Message ids.
  enum 
  {
    ID_NEW = FoxMainWindow::ID_LAST,
    ID_LAST
  };

  FoxMdiMainWindow ( 
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

  // Get the client area.
  const FXMDIClient *       getClientArea() const { return _clientArea; }
  FXMDIClient *             getClientArea()       { return _clientArea; }

  // Get the MDI menu.
  const FXMDIMenu *         getMdiMenu() const { return _mdiMenu; }
  FXMDIMenu *               getMdiMenu()       { return _mdiMenu; }

  // Two-step construction.
  virtual bool        init();

  // Message handlers.
  long                onCommandNew ( FXObject *, FXSelector, void * );

protected:

  SlAutoPtr<FXMDIClient> _clientArea;
  SlAutoPtr<FXMDIMenu> _mdiMenu;

  FoxMdiMainWindow();
  virtual ~FoxMdiMainWindow();

  FXDECLARE ( FoxMdiMainWindow );
};


}; // namespace CadKit


#endif _CADKIT_GUI_FOX_MAIN_MDI_WINDOW_H_
