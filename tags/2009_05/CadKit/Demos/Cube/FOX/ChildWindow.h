
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  MDI Child window.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DEMO_FOX_CUBE_MDI_CHILD_WINDOW_H_
#define _CADKIT_DEMO_FOX_CUBE_MDI_CHILD_WINDOW_H_

#include "SceneGraph/FOX/SgFoxChildWindow.h"


namespace CadKit
{
class MainWindow;

class ChildWindow : public SgFoxChildWindow 
{
public:

  ChildWindow ( const FXString &name, MainWindow *parent );
  ChildWindow();
  virtual ~ChildWindow();

  FXDECLARE ( ChildWindow );
};


}; // namespace CadKit


#endif _CADKIT_DEMO_FOX_CUBE_MDI_CHILD_WINDOW_H_
