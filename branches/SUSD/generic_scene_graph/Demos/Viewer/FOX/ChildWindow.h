
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

#ifndef _GENERIC_SCENE_GRAPH_DEMO_FOX_VIEWER_MDI_CHILD_WINDOW_H_
#define _GENERIC_SCENE_GRAPH_DEMO_FOX_VIEWER_MDI_CHILD_WINDOW_H_


class MainWindow;


class ChildWindow : public GSG::FOX::MdiChildWindow 
{
public:

  typedef GSG::FOX::MdiChildWindow BaseClass;

  ChildWindow ( const FXString &name, MainWindow *parent );
  ChildWindow();
  virtual ~ChildWindow();

  // Create.
  virtual void        create();

  FXDECLARE ( ChildWindow );
};


#endif _GENERIC_SCENE_GRAPH_DEMO_FOX_VIEWER_MDI_CHILD_WINDOW_H_
