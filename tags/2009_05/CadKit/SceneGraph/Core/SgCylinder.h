
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgCylinder.h: Cylinder class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_CYLINDER_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_CYLINDER_H_

#include "SgPrimitive.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlVec3.h"
#endif


namespace CadKit
{
class SG_API SgCylinder : public SgPrimitive
{
public:

  SlVec3f baseCenter;
  SlFloat32 baseRadius;
  SlFloat32 topRadius;
  SlFloat32 height;

  SgCylinder();
  SgCylinder ( const SlVec3f &baseCenter, const SlFloat32 &baseRadius, const SlFloat32 &topRadius, const SlFloat32 &height );

protected:

  virtual ~SgCylinder();

  SG_DECLARE_DYNAMIC_NODE(SgCylinder,0x0000100E);
};
};

#endif
