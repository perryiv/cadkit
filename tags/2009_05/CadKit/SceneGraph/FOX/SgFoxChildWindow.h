
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OpenGL child window for the GUI-FOX library.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_GUI_FOX_OPENGL_CHILD_WINDOW_H_
#define _CADKIT_GUI_FOX_OPENGL_CHILD_WINDOW_H_

#include "SgFoxApi.h"

#ifdef _WIN32
# define NOMINMAX // Fox includes <windows.h>
#endif

#include <fx.h>
#include <fx3d.h>

#include "SceneGraph/Core/SgGroup.h"

#include "Standard/SlAutoPtr.h"


namespace CadKit
{
class SgFoxView;

class SG_FOX_API SgFoxChildWindow : public FXMDIChild
{
public:

  SgFoxChildWindow ( 
    FXMDIClient *parent,
    const FXString &name,
    FXIcon *icon = NULL,
    FXMenuPane *menu = NULL,
    FXuint options = 0,
    FXint x = 0,
    FXint y = 0,
    FXint w = 0,
    FXint h = 0 );
  SgFoxChildWindow();
  virtual ~SgFoxChildWindow();

  // Need to call after init if GUI is already up and running.
  virtual void        create();

  // Two-step construction.
  bool                init();

  // Is it created?
  bool                isCreated() const { return ( 0x0 != this->id() ); }

protected:

  static SlAutoPtr<FXGLVisual> _visual;
  SlAutoPtr<SgFoxView> _view;
  SgGroup::Ptr _root;

  FXDECLARE ( SgFoxChildWindow );
};


}; // namespace CadKit


#endif _CADKIT_GUI_FOX_OPENGL_CHILD_WINDOW_H_
