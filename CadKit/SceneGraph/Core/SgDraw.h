
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgDraw.h: Misc draw functions.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_MISC_DRAW_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_MISC_DRAW_H_

#include "SgApi.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlVec2.h"
# include "Standard/SlVec3.h"
# include "Standard/SlBase.h"
#endif


namespace CadKit
{
class SgSeparator;
class SgGroup;
class SgPrimitive;

class SG_API SgDraw
{
public:

  // These are highly un-optimized drawing functions that are intended
  // for use in debugging your application.

  static SgSeparator *drawLine ( const SlVec3f &pt0, const SlVec3f &pt1 );
  static SgSeparator *drawLine ( const SlVec2f &pt0, const SlVec2f &pt1 );
  static SgSeparator *drawLine ( const float &x0, const float &y0, const float &x1, const float &y1 );
  static SgSeparator *drawLine ( const float &x0, const float &y0, const float &z0, const float &x1, const float &y1, const float &z1 );

  static SgSeparator *drawLine ( const SlVec3d &pt0, const SlVec3d &pt1 );
  static SgSeparator *drawLine ( const SlVec2d &pt0, const SlVec2d &pt1 );
  static SgSeparator *drawLine ( const double & x0, const double & y0, const double & x1, const double & y1 );
  static SgSeparator *drawLine ( const double & x0, const double & y0, const double & z0, const double & x1, const double & y1, const double & z1 );

  static bool draw2dPoints (
    // The data points to draw, all the x's, all the y's.
    const double *points, 

    // The number of data points to draw.
    const unsigned long &numPoints, 

    // Pass the type of an SgPrimitive node.
    const SlType *primitiveType, 

    // The size of the points.
    const float &size, 

    // Pass true if you want the "size" to be constant relative to the screen.
    bool setScale,

    // Pass true is there will only be a uniform scale. This flag is ignored if "setScale" is false.
    bool uniformScale,

    // Pass true if the scene will be rotated. This flag is ignored if "setScale" is false.
    bool willRotate,

    // The z-coordinate of the data points (there is
    const float &z,

    // Add the scene to this group.
    SgGroup *group );

  static bool drawPointsAsPixels (
    // The data points to draw, all the x's, all the y's, ...
    const double *points, 

    // The number of data points to draw.
    const unsigned long &numPoints, 

    // The dimension of the points. Has to be >= 2. If > 3 then first 3 dimensions used.
    const unsigned long &dimension,

    // Add the sceen to this group
    SgGroup *group );

protected:

  static void _setCenter ( SgPrimitive *prim, const float &x, const float &y, const float &z );
  static void _setSize ( SgPrimitive *prim, const float &size );
};
};

#endif
