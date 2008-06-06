
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgCone.h: Cone class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_CONE_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_CONE_H_

#include "SgPrimitive.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlVec3.h"
#endif


namespace CadKit
{
class SG_API SgCone : public SgPrimitive
{
public:

  SlVec3f baseCenter;
  SlFloat32 radius;
  SlFloat32 height;

  SgCone();
  SgCone ( const SlVec3f &baseCenter, const SlFloat32 &radius, const SlFloat32 &height );

protected:

  virtual ~SgCone();

  SG_DECLARE_DYNAMIC_NODE(SgCone,0x00001009);
};
};

#endif
