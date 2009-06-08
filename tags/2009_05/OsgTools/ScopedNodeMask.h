
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_SCOPED_NODE_MAKE_H__
#define __OSGTOOLS_SCOPED_NODE_MAKE_H__

#include "osg/Node"

namespace OsgTools {


class ScopedNodeMask
{
public:
  ScopedNodeMask ( osg::Node& node, unsigned int mask ) :
    _node ( node ),
    _mask ( node.getNodeMask() )
  {
    node.setNodeMask ( mask );
  }

  virtual ~ScopedNodeMask()
  {
    _node.setNodeMask ( _mask );
  }
private:
  osg::Node &_node;
  unsigned int _mask;
};


}


#endif // __OSGTOOLS_SCOPED_NODE_MAKE_H__
