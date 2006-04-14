
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
#include "SceneTree.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IFoxAddTab.h"
#include "Usul/Interfaces/IUpdateTreeControls.h"
#include "Usul/Interfaces/IPlugin.h"


namespace osg { class Drawable; class Node; }


class SceneTreeComponent : public Usul::Base::Referenced,
                           public Usul::Interfaces::IFoxAddTab,
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

  virtual std::string getPluginName() const { return "Scene Tree"; }

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IFoxAddTab
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void foxAddTab( Usul::Interfaces::IUnknown *caller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IUpdateSceneTreeControls
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void updateTreeControls( osg::Node *scene  );

protected:

  // Do not copy.
  SceneTreeComponent ( const SceneTreeComponent & );
  SceneTreeComponent &operator = ( const SceneTreeComponent & );

  /// Use reference counting.
  virtual ~SceneTreeComponent();

  Usul::Interfaces::IUnknown::QueryPtr _caller;

  SceneTree *_sceneTree;
};


#endif // _SCENE_TREE_COMPONENT_CLASS_H_
