
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

#ifndef _CADKIT_DEMO_FOX_CUBE_MAIN_WINDOW_H_
#define _CADKIT_DEMO_FOX_CUBE_MAIN_WINDOW_H_

#include "GUI/FOX/FoxMdiMainWindow.h"


namespace CadKit
{
class MainWindow : public FoxMdiMainWindow 
{
public:

  MainWindow ( FXApp *app, const FXString &name, const FXint &w, const FXint &h );
  MainWindow();
  virtual ~MainWindow();

  // Message handlers.
  long onCommandNew ( FXObject *, FXSelector, void * );

  FXDECLARE ( MainWindow );
};


}; // namespace CadKit


#endif _CADKIT_DEMO_FOX_CUBE_MAIN_WINDOW_H_
