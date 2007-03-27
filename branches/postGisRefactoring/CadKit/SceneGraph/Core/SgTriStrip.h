
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgTriStrip.h: Line set class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_TRIANGLE_STRIP_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_TRIANGLE_STRIP_H_

#include "SgVertexSet.h"

namespace CadKit
{
class SG_API SgTriStrip : public SgVertexSet
{
public:

  SgTriStrip();

protected:

  virtual ~SgTriStrip();

  SG_DECLARE_DYNAMIC_NODE(SgTriStrip,0x00001034);
};
};

#endif
