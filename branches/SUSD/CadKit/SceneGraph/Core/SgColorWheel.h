
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgColorWheel.h: Color wheel node.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_COLOR_WHEEL_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_COLOR_WHEEL_H_

#include "SgShape.h"

namespace CadKit
{
class SG_API SgColorWheel : public SgShape
{
public:

  SlFloat32 radius;
  SlFloat32 intersectRadius;
  SlInt32 numSlices;

  SgColorWheel();
  SgColorWheel ( const SlFloat32 &radius, const SlFloat32 &intersectRadius, const SlInt32 &numSlices );

protected:

  virtual ~SgColorWheel();

  SG_DECLARE_DYNAMIC_NODE(SgColorWheel,0x00001008);
};
};

#endif
