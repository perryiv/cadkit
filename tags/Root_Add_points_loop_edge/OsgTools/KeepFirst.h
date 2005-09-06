
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor that hangs on to the first non-null node.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_GENERIC_KEEP_FIRST_H_
#define _OSG_TOOLS_GENERIC_KEEP_FIRST_H_

#include <osg/ref_ptr>


namespace OsgTools {


template < class NodeType > struct KeepFirst
{
  KeepFirst() : _node ( 0x0 )
  {
  }
  KeepFirst ( const KeepFirst &k ) : _node ( k._node )
  {
  }
  KeepFirst &operator = ( const KeepFirst &k )
  {
    _node = k._node;
    return *this;
  }
  void operator () ( NodeType *node )
  {
    if ( !_node.valid() )
      _node = node;
  }
  const NodeType *node() const
  {
    return _node.get();
  }
  NodeType *node()
  {
    return _node.get();
  }

private:

  osg::ref_ptr<NodeType> _node;
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_GENERIC_KEEP_FIRST_H_
