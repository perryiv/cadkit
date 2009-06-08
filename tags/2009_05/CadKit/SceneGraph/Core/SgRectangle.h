
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgRectangle.h: Rectangle class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_RECTANGLE_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_RECTANGLE_H_

#include "SgPrimitive.h"
#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlVec2.h"
#endif

namespace CadKit
{
class SG_API SgRectangle : public SgPrimitive
{
public:

  enum Style 
  {     
    POLYGON,  // Draw a polygon for the rectangle.
    BORDER    // Draw 4 lines for the border of the rectangle.
  };

  SlVec2f corner0;
  SlVec2f corner1;
  SlFloat32 z;
  Style style;

  SgRectangle();

protected:

  virtual ~SgRectangle();

  SG_DECLARE_DYNAMIC_NODE(SgRectangle,0x00001024);
};
};

#endif
