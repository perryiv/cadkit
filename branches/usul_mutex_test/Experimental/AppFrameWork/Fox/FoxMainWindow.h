
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  MDI main window.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_GUI_SERVER_MAIN_WINDOW_CLASS_H_
#define _FOX_GUI_SERVER_MAIN_WINDOW_CLASS_H_

#include "AppFrameWork/Fox/CompileGuard.h"
#include "AppFrameWork/Core/MainWindow.h"

#include "FoxTools/Headers/MainWindow.h"


class FoxMainWindow : public AFW::Core::MainWindow,
                      public FX::FXObject
{
public:

  // Useful typedefs.
  typedef AFW::Core::MainWindow BaseClass;

  // Constructor
  FoxMainWindow();

protected:

  // Use reference counting.
  virtual ~FoxMainWindow();

private:

  // No copying.
  FoxMainWindow ( const FoxMainWindow & );
  FoxMainWindow &operator = ( const FoxMainWindow & );

  FX::FXMainWindow *_mainWindow;

  AFW_DECLARE_WINDOW ( FoxMainWindow );
};


#endif //_FOX_GUI_SERVER_MAIN_WINDOW_CLASS_H_
