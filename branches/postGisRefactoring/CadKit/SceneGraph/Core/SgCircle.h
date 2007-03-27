
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgCircle.h: Circle class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_CIRCLE_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_CIRCLE_H_

#include "SgPrimitive.h"
#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlVec3.h"
#endif

namespace CadKit
{
class SG_API SgCircle : public SgPrimitive
{
public:

  SlVec3f center;
  SlFloat32 radius;

  SgCircle();
  SgCircle ( const SlVec3f &center, const SlFloat32 &radius );

protected:

  virtual ~SgCircle();

  SG_DECLARE_DYNAMIC_NODE(SgCircle,0x00001007);
};
};

#endif
