
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

#ifndef _OPEN_SCENE_GRAPH_FOX_BINDING_MAIN_MDI_WINDOW_H_
#define _OPEN_SCENE_GRAPH_FOX_BINDING_MAIN_MDI_WINDOW_H_

#include "OsgFox/Core/MainWindow.h"

namespace FX { class FXMDIClient; class FXMDIMenu; }

#include "Usul/Interfaces/IFoxClientArea.h"
#include "Usul/Interfaces/IFoxMDIMenu.h"

namespace FoxTools { namespace Windows { class MdiChildWindow; } }

namespace OsgFox {

namespace Documents { class Document; }

namespace Core {

class OSG_FOX_CORE_EXPORT MdiMainWindow : public MainWindow,
                                          public Usul::Interfaces::IFoxClientArea,
                                          public Usul::Interfaces::IFoxMDIMenu
{
public:
  // Useful typedefs.
  typedef MainWindow BaseClass;
  typedef FoxTools::Windows::MdiChildWindow MdiChildWindow;
  typedef Usul::Interfaces::IViewer IViewer;

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  MdiMainWindow ( 
    FX::FXApp *app,
    const FX::FXString &name,
    FX::FXIcon *icon = 0x0,
    FX::FXIcon *mi = 0x0,
    FX::FXuint options = FX::DECOR_ALL );

  // Return the active child, which may be null.
  const MdiChildWindow *                activeChild() const;
  MdiChildWindow *                      activeChild();

  // Usul::Interfaces::IFoxClientArea
  // Get the client area.
  virtual const FX::FXMDIClient *       clientArea() const { return _clientArea; }
  virtual FX::FXMDIClient *             clientArea()       { return _clientArea; }

  // Usul::Interfaces::IFoxMDIMenu
  // Get the MDI menu.
  virtual const FX::FXMDIMenu *         mdiMenu() const { return _mdiMenu; }
  virtual FX::FXMDIMenu *               mdiMenu()       { return _mdiMenu; }

  // GUI update.
  long                                  onUpdateNeedingChild     ( FX::FXObject *, FX::FXSelector, void * );

protected:

  MdiMainWindow();
  virtual ~MdiMainWindow();

  virtual bool                          _exiting();

  virtual void                          _fullScreenStart();
  virtual void                          _fullScreenEnd();

  void                                  _initWindowMenu();

private:

  FX::FXMDIClient *_clientArea;
  FX::FXMDIMenu *_mdiMenu;

  bool _wasMaximized;

  FXDECLARE ( MdiMainWindow );
};

} // namespace Core
} // namespace OsgFox


#endif //_OPEN_SCENE_GRAPH_FOX_BINDING_MAIN_MDI_WINDOW_H_
