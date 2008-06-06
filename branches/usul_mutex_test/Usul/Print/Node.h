
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions for printing scene-graph nodes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PRINT_SCENE_GRAPH_NODES_H_
#define _USUL_PRINT_SCENE_GRAPH_NODES_H_


namespace Usul {
namespace Print {


///////////////////////////////////////////////////////////////////////////////
//
//  Print a node.
//
///////////////////////////////////////////////////////////////////////////////

namespace OSG
{
  template
  <
    class StringType_, 
    class NodeType_, 
    class StreamType_
  > 
  inline void node ( const StringType_ &message, 
                    const NodeType_ *node, 
                    StreamType_ &out )
  {
    if ( node )
    {
      out << message
          << node
          << ", type = "
          << node->className()
          << ", name = "
          << node->getName()
          << '\n';
    }
  }
} // namespace OSG


} // namespace Print
} // namespace Usul


#endif // _USUL_PRINT_SCENE_GRAPH_NODES_H_
