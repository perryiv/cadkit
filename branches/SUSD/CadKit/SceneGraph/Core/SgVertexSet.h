
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgVertexSet.h: Base class for all vertex based classes.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_VERTEX_SET_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_VERTEX_SET_H_

#include "SgShape.h"

namespace CadKit
{
class SG_API SgVertexSet : public SgShape
{
protected:

  SgVertexSet();
  virtual ~SgVertexSet();

  SG_DECLARE_NODE(SgVertexSet,0x00001037);
};
};

#endif
