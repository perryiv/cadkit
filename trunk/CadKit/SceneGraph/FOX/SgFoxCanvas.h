
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OpenGL canvas for the GUI-FOX library.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_GUI_FOX_OPENGL_CANVAS_H_
#define _CADKIT_GUI_FOX_OPENGL_CANVAS_H_

#include "SgFoxApi.h"

#ifdef _WIN32
# define NOMINMAX // Fox includes <windows.h>
#endif

#include <fx.h>
#include <fx3d.h>


namespace CadKit
{
class SG_FOX_API SgFoxCanvas : public FXGLCanvas
{
 public:

  SgFoxCanvas ( FXComposite *parent, FXGLVisual *visual );
  virtual ~SgFoxCanvas();

  // Is it created?
  bool                isCreated() const { return ( 0x0 != this->id() ); }

  // Message map(s).
  long                onLeftBtnPress ( FXObject *object, FXSelector id, void *data );

 protected:

  SgFoxCanvas();

  FXDECLARE ( SgFoxCanvas );
};


};


#endif // _CADKIT_GUI_FOX_OPENGL_CANVAS_H_
