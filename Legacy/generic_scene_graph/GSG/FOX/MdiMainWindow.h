
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  MDI Main window.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_FOX_BINDING_MAIN_MDI_WINDOW_H_
#define _GENERIC_SCENE_GRAPH_FOX_BINDING_MAIN_MDI_WINDOW_H_

#include "GSG/FOX/MainWindow.h"


namespace GSG {
namespace FOX {


class GSG_FOX_EXPORT MdiMainWindow : public MainWindow 
{
public:

  // Message ids.
  enum 
  {
    ID_NEW = MainWindow::ID_LAST,
    ID_LAST
  };

  MdiMainWindow ( 
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

  // Get the client area.
  const FXMDIClient *       clientArea() const { return _clientArea.get(); }
  FXMDIClient *             clientArea()       { return _clientArea.get(); }

  // Get the MDI menu.
  const FXMDIMenu *         mdiMenu() const { return _mdiMenu.get(); }
  FXMDIMenu *               mdiMenu()       { return _mdiMenu.get(); }

  // Message handlers.
  long                      onCommandNew ( FXObject *, FXSelector, void * );

protected:

  MdiMainWindow();
  virtual ~MdiMainWindow();

private:

  typedef std::auto_ptr < FXMDIClient > FXMDIClientPtr;
  typedef std::auto_ptr < FXMDIMenu   > FXMDIMenuPtr;

  FXMDIClientPtr _clientArea;
  FXMDIMenuPtr _mdiMenu;

  FXDECLARE ( MdiMainWindow );
};


}; // namespace FOX
}; // namespace GSG


#endif _GENERIC_SCENE_GRAPH_FOX_BINDING_MAIN_MDI_WINDOW_H_
