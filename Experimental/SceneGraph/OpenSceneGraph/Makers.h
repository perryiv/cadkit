
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The maker classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_OSG_MAKER_CLASSES_H_
#define _SCENE_GRAPH_OSG_MAKER_CLASSES_H_

#include "SceneGraph/OpenSceneGraph/Node.h"

#include "Usul/Base/Referenced.h"

#include "osg/Node"
#include "osg/ref_ptr"


namespace SceneGraph {
namespace OSG {


///////////////////////////////////////////////////////////////////////////////
//
//  Base class for creating objects.
//
///////////////////////////////////////////////////////////////////////////////

struct BaseNodeMaker : public Usul::Base::Referenced
{
  typedef Usul::Base::Referenced BaseClass;
  USUL_DECLARE_REF_POINTERS ( BaseNodeMaker );
  virtual Node::RefPtr operator() ( osg::ref_ptr<osg::Node> wrapMe )
  {
    throw std::runtime_error ( "Error 4960271020: Should not be here" );
  }
protected:
  BaseNodeMaker() : BaseClass(){}
  virtual ~BaseNodeMaker(){}
private:
  BaseNodeMaker ( const BaseNodeMaker & );
  BaseNodeMaker &operator = ( const BaseNodeMaker & );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Class for creating objects.
//
///////////////////////////////////////////////////////////////////////////////

template < class NodeType, class OsgType > struct DerivedNodeMaker : public BaseNodeMaker
{
  typedef BaseNodeMaker BaseClass;
  DerivedNodeMaker() : BaseClass(){}
  virtual Node::RefPtr operator() ( osg::ref_ptr<osg::Node> wrapMe )
  {
    osg::ref_ptr<OsgType> derivedOsg ( dynamic_cast < OsgType * > ( wrapMe.get() ) );
    return Node::RefPtr ( ( true == derivedOsg.valid() ) ? new NodeType ( derivedOsg ) : 0x0 );
  }
protected:
  virtual ~DerivedNodeMaker(){}
private:
  DerivedNodeMaker ( const DerivedNodeMaker & );
  DerivedNodeMaker &operator = ( const DerivedNodeMaker & );
};


} // namespace OSG
} // namespace SceneGraph


#endif // _SCENE_GRAPH_OSG_MAKER_CLASSES_H_
