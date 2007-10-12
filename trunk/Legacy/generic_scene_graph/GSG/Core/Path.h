
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A node path.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_PATH_H_
#define _GENERIC_SCENE_GRAPH_CORE_PATH_H_

#include "GSG/Core/Node.h"

#include <stack>


namespace GSG {

class Node;


class GSG_CORE_EXPORT Path : public Referenced
{
public:

  GSG_DECLARE_REFERENCED ( Path );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Path, Referenced );

  explicit Path();
  Path ( const Path &p );

  virtual void    push ( Node *n );
  virtual void    pop();

protected:

  virtual ~Path();

  typedef std::stack < Node::ValidPtr > Stack;
  Stack _stack;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_PATH_H_
