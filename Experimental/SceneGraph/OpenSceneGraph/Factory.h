
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_OSG_FACTORY_CLASS_H_
#define _SCENE_GRAPH_OSG_FACTORY_CLASS_H_

#include "SceneGraph/OpenSceneGraph/Node.h"
#include "SceneGraph/OpenSceneGraph/Makers.h"

#include "Usul/Factory/BaseFactory.h"
#include "Usul/Preprocess/Singleton.h"

#include "osg/Node"
#include "osg/ref_ptr"

#include <map>


namespace SceneGraph {
namespace OSG {


class Factory : public SceneGraph::OSG::Object
{
public:

  // Typedefs.
  typedef SceneGraph::OSG::Object BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef osg::ref_ptr<osg::Node> NodePtr;
  typedef std::map<std::string,BaseNodeMaker::RefPtr> NodeMakers;

  // Create the node.
  IUnknown::RefPtr              createObject ( NodePtr );
  IUnknown::RefPtr              createObject ( unsigned long iid );

  // Construction and destruction.
  Factory();
  ~Factory();

protected:

  void                          _destroy();

private:

  // Do not copy.
  Factory ( const Factory & );
  Factory &operator = ( const Factory & );

  NodeMakers _nodeMakers;
};


namespace Singleton { class USUL_DEFINE_SINGLETON ( Factory, SceneGraph::OSG::Factory ); }


} // namespace OSG
} // namespace SceneGraph


#endif // _SCENE_GRAPH_OSG_FACTORY_CLASS_H_
