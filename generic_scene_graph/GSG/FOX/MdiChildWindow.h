
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

#ifndef _GENERIC_SCENE_GRAPH_FOX_BINDING_CHILD_WINDOW_H_
#define _GENERIC_SCENE_GRAPH_FOX_BINDING_CHILD_WINDOW_H_

#include "GSG/FOX/FoxDll.h"

#include "GSG/Core/Group.h"

#include "fx.h"
#include "fx3d.h"


namespace GSG {
namespace FOX {


class View;

class GSG_FOX_EXPORT MdiChildWindow : public FXMDIChild
{
public:

  typedef FXMDIChild BaseClass;

  MdiChildWindow ( 
    FXMDIClient *parent,
    const FXString &name,
    FXIcon *icon = 0x0,
    FXMenuPane *menu = 0x0,
    FXuint options = 0,
    FXint x = 0,
    FXint y = 0,
    FXint w = 0,
    FXint h = 0 );
  MdiChildWindow();
  virtual ~MdiChildWindow();

  // Create.
  virtual void        create();

  // Is it created?
  bool                isCreated() const;

  // Access to the root.
  const Group *       root() const;
  Group *             root();

  // Access to the view.
  const View *        view() const;
  View *              view();

private:

  typedef std::auto_ptr < FXGLVisual > FXGLVisualPtr;
  typedef std::auto_ptr < View       > ViewPtr;

  static FXGLVisualPtr _visual;
  ViewPtr _view;
  Group::Ptr _root;

  FXDECLARE ( MdiChildWindow );
};


}; // namespace FOX
}; // namespace GSG


#endif _GENERIC_SCENE_GRAPH_FOX_BINDING_CHILD_WINDOW_H_
