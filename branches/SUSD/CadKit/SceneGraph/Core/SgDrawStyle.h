
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgDrawStyle.h: Drawing style class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_DRAW_STYLE_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_DRAW_STYLE_H_

#include "SgAttribute.h"

namespace CadKit
{
class SG_API SgDrawStyle : public SgAttribute
{
public:

  enum PolygonStyle
  {     
    FILLED, // Draw filled regions.
    LINES,  // Draw only outlines (wire frame).
    POINTS  // Draw points at vertices.
  };

  enum Side
  {     
    FRONT,          // Apply the draw style to the front.
    BACK,           // Apply the draw style to the back.
    FRONT_AND_BACK  // Apply the draw style to the front and back.
  };

  PolygonStyle polygonStyle;
  Side side;
  SlFloat32 pointSize;
  SlFloat32 lineWidth;
  SlInt32 lineStippleFactor;
  SlUint16 lineStipplePattern;

  SgDrawStyle();

  virtual bool          isEqualVisibleProperties ( const SgNode &node ) const;

protected:

  virtual ~SgDrawStyle();

  SG_DECLARE_DYNAMIC_NODE ( SgDrawStyle, 1033858462 );
};
};

#endif
