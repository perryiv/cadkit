///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __LOOP_AND_TRIANGLE_ENUMERATIONS_H__
#define __LOOP_AND_TRIANGLE_ENUMERATIONS_H__


namespace OsgTools
{
  namespace Triangles
  {
    enum AxisList 
    {
      X_AXIS,
      Y_AXIS,
      Z_AXIS,
      NO_AXIS
    };

    enum EdgeList 
    {
      ZMIN_YMIN = 1,
      XMIN_YMIN = 2,
      ZMAX_YMIN = 3,
      XMAX_YMIN = 4,
      ZMIN_YMAX = 5,
      XMIN_YMAX = 6,
      ZMAX_YMAX = 7,
      XMAX_YMAX = 8,
      XMAX_ZMIN = 9,
      XMIN_ZMIN = 10,
      XMIN_ZMAX = 11,
      XMAX_ZMAX = 12
    };   
    
    enum OsgBoundingBox
    {
      XMIN_YMIN_ZMIN = 0,
      XMAX_YMIN_ZMIN = 1,
      XMIN_YMAX_ZMIN = 2,
      XMAX_YMAX_ZMIN = 3,
      XMIN_YMIN_ZMAX = 4,
      XMAX_YMIN_ZMAX = 5,
      XMIN_YMAX_ZMAX = 6,
      XMAX_YMAX_ZMAX = 7     
    };

    ///////////////////////////////////////////////////////////////////////////
    //
    //  These values were created by adding the values of the 3 edges that connect
    //  to form the corner point. Nicely enough, these values are unique
    //
    ///////////////////////////////////////////////////////////////////////////

    enum CornerEdge
    {
      CE_XMIN_YMIN_ZMIN = 13,
      CE_XMAX_YMIN_ZMIN = 14,
      CE_XMIN_YMAX_ZMIN = 21,
      CE_XMAX_YMAX_ZMIN = 22,
      CE_XMIN_YMIN_ZMAX = 16,
      CE_XMAX_YMIN_ZMAX = 19,
      CE_XMIN_YMAX_ZMAX = 24,
      CE_XMAX_YMAX_ZMAX = 27     
    };
  }
}


#endif //__LOOP_AND_TRIANGLE_ENUMERATIONS_H__
