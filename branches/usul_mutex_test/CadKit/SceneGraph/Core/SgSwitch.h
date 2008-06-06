
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgSwitch.h: A group node that renders only the specified children.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_SWITCH_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_SWITCH_H_

#include "SgGroup.h"

namespace CadKit
{
class SG_API SgSwitch : public SgGroup
{
public:

  SlUint32 whichChild;

  enum TraversalMode
  {
    ALL,  // Behaves just like SgGroup.
    ONE,  // Traverses the child specified by "whichChild".
    NONE  // Doesn't traverse.
  };

  TraversalMode traversal;

  SgSwitch();

protected:

  virtual ~SgSwitch();

  SG_DECLARE_DYNAMIC_NODE ( SgSwitch, 1033948671 );
};
};

#endif
