
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgCube.h: Cube class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_CUBE_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_CUBE_H_

#include "SgPrimitive.h"
#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlVec3.h"
#endif

namespace CadKit
{
class SG_API SgCube : public SgPrimitive
{
public:

  SlVec3f center;
  SlFloat32 size;

  SgCube();
  SgCube ( const SlVec3f &center, const SlFloat32 &size );

protected:

  virtual ~SgCube();

  SG_DECLARE_DYNAMIC_NODE(SgCube,0x0000100D);
};
};

#endif
