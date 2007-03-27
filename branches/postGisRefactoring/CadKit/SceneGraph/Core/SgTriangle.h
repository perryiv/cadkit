
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgTriangle.h: Square class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_TRIANGLE_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_TRIANGLE_H_

#include "SgPrimitive.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlVec3.h"
#endif


namespace CadKit
{
class SG_API SgTriangle : public SgPrimitive
{
public:

  SlVec3f center;
  SlFloat32 size;

  SgTriangle();
  SgTriangle ( const SlVec3f &center, const SlFloat32 &s );

protected:

  virtual ~SgTriangle();

  SG_DECLARE_DYNAMIC_NODE(SgTriangle,0x00001033);
};
};

#endif
