
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgPointSet.h: Point set class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_POINT_SET_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_POINT_SET_H_

#include "SgVertexSet.h"

namespace CadKit
{
class SG_API SgPointSet : public SgVertexSet
{
public:

  SgPointSet();

protected:

  virtual ~SgPointSet();

  SG_DECLARE_DYNAMIC_NODE(SgPointSet,0x00001021);
};
};

#endif
