
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

#ifndef _FOX_SAL_VIEWER_MAIN_WINDOW_H_
#define _FOX_SAL_VIEWER_MAIN_WINDOW_H_

#include "SAL/FOX/MdiMainWindow.h"


class MainWindow : public GSG::FOX::MdiMainWindow 
{
public:

  typedef GSG::FOX::MdiMainWindow BaseClass;

  MainWindow ( FXApp *app, const FXString &name, const FXint &w, const FXint &h );
  MainWindow();
  virtual ~MainWindow();

  // Message handlers.
  long onCommandNew ( FXObject *, FXSelector, void * );

  FXDECLARE ( MainWindow );
};


#endif _FOX_SAL_VIEWER_MAIN_WINDOW_H_
