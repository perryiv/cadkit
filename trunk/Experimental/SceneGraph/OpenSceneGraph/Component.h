
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_OSG_COMPONENT_CLASS_H_
#define _SCENE_GRAPH_OSG_COMPONENT_CLASS_H_

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ISceneGraph.h"


namespace SceneGraph {
namespace OSG {


class Component : public Usul::Base::Object,
                  public Usul::Interfaces::IPlugin,
                  public Usul::Interfaces::SceneGraph::IFactory
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Component );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  Component();

  // Usul::Interfaces::IPlugin
  virtual std::string           getPluginName() const { return "Scene Graph OSG"; }

  // Usul::Interfaces::SceneGraph::IFactory
  virtual IUnknown::RefPtr      createObject ( unsigned long iid ) const;

protected:

  // Use reference counting.
  virtual ~Component();

  void                          _destroy();

private:

  // Do not copy.
  Component ( const Component & );
  Component &operator = ( const Component & );
};


} // namespace OSG
} // namespace SceneGraph


#endif // _SCENE_GRAPH_OSG_COMPONENT_CLASS_H_
