
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Convert functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_OSG_CONVERT_CLASS_H_
#define _SCENE_GRAPH_OSG_CONVERT_CLASS_H_

#include "Usul/Convert/Generic.h"
#include "Usul/Interfaces/ISceneGraph.h"

#include "SceneGraph/OpenSceneGraph/Node.h"

#include "osg/Node"
#include "osg/ref_ptr"


namespace Usul
{
  namespace Convert
  {
    template <> struct Type < Usul::Interfaces::IUnknown, osg::Node >
    {
      typedef Usul::Interfaces::IUnknown IUnknown;
      typedef Usul::Interfaces::SceneGraph::IChild IChild;
      typedef SceneGraph::OSG::Node Node;
      typedef osg::ref_ptr<osg::Node> NodePtr;
      typedef Type < IUnknown, osg::Node > ThisType;

      static void convert ( IUnknown::RefPtr from, NodePtr &to )
      {
        IChild::QueryPtr child ( from );
        Node::RefPtr n ( dynamic_cast < Node * > ( child.get() ) );
        to = ( ( true == n.valid() ) ? n->node() : NodePtr ( 0x0 ) );
      }

      static NodePtr convert ( IUnknown::RefPtr from )
      {
        NodePtr to ( 0x0 );
        ThisType::convert ( from, to );
        return to;
      }
    };
  }
}


#endif // _SCENE_GRAPH_OSG_CONVERT_CLASS_H_
