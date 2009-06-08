
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgSquare.h: Square class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_SQUARE_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_SQUARE_H_

#include "SgPrimitive.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlVec3.h"
#endif


namespace CadKit
{
class SG_API SgSquare : public SgPrimitive
{
public:

  SlVec3f center;
  SlFloat32 size;

  SgSquare();
  SgSquare ( const SlVec3f &center, const SlFloat32 &size );

protected:

  virtual ~SgSquare();

  SG_DECLARE_DYNAMIC_NODE(SgSquare,0x0000102E);
};
};

#endif
