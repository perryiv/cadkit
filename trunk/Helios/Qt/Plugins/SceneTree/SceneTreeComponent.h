
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_TREE_COMPONENT_CLASS_H_
#define _SCENE_TREE_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/GUI/IAddDockWindow.h"
#include "Usul/Interfaces/GUI/IUpdateTreeControls.h"
#include "Usul/Interfaces/IPlugin.h"

class QTreeWidget; class QDockWidget;

namespace osg { class Node; }


class SceneTreeComponent : public Usul::Base::Referenced,
                           public Usul::Interfaces::IAddDockWindow,
                           public Usul::Interfaces::IUpdateTreeControls,
                           public Usul::Interfaces::IPlugin
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SceneTreeComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  SceneTreeComponent();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IPlugin
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual std::string getPluginName() const { return "Scene Tree Qt"; }

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IFoxAddTab
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void addDockWindow ( Usul::Interfaces::IUnknown *caller = 0x0 );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IUpdateSceneTreeControls
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void updateTreeControls( osg::Node *scene );

protected:

  // Do not copy.
  SceneTreeComponent ( const SceneTreeComponent & );
  SceneTreeComponent &operator = ( const SceneTreeComponent & );

  /// Use reference counting.
  virtual ~SceneTreeComponent();

  Usul::Interfaces::IUnknown::QueryPtr _caller;

  QDockWidget     *_dock;
  QTreeWidget     *_sceneTree;
};


#endif // _SCENE_TREE_COMPONENT_CLASS_H_
