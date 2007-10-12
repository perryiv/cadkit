
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OpenGL canvas.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_FOX_BINDING_CANVAS_H_
#define _GENERIC_SCENE_GRAPH_FOX_BINDING_CANVAS_H_

#include "GSG/FOX/FoxDll.h"

#include "fx.h"
#include "fx3d.h"


namespace GSG {
namespace FOX {


class GSG_FOX_EXPORT Canvas : public FXGLCanvas
{
 public:

  Canvas ( FXComposite *parent, FXGLVisual *visual );
  virtual ~Canvas();

  // Is it created?
  bool                isCreated() const;

  // Message map(s).
  long                onLeftBtnPress ( FXObject *object, FXSelector id, void *data );

 protected:

  Canvas();

  FXDECLARE ( Canvas );
};


}; // namespace FOX
}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_FOX_BINDING_CANVAS_H_
