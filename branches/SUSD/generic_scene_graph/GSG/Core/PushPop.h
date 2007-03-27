
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Pushes and pops the node-stack. For exception safety.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_PUSH_POP_H_
#define _GENERIC_SCENE_GRAPH_CORE_PUSH_POP_H_


namespace GSG {


class Visitor;
class Node;


namespace Detail {


struct PushPop
{
  PushPop ( Visitor &v, Node *n );
  ~PushPop();

private:

  Visitor &_v;
};


}; // namespace Detail
}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_PUSH_POP_H_
