
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgVisitorMacros.h: Macros that are used in visitor classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_VISITOR_MACROS_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_VISITOR_MACROS_H_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlRefBase.h"
#endif


///////////////////////////////////////////////////////////////////////////////
//
// Put this in your abstract visitor's class definition.
//
///////////////////////////////////////////////////////////////////////////////

#define SG_DECLARE_VISITOR(class_name, id) \
  SL_DECLARE_REFERENCE_POINTER(class_name); \
  SL_DECLARE_CLASS(class_name, id)


///////////////////////////////////////////////////////////////////////////////
//
// Put this in your concrete visitor's class definition.
//
///////////////////////////////////////////////////////////////////////////////

#define SG_DECLARE_DYNAMIC_VISITOR(class_name, id) \
  SL_DECLARE_REFERENCE_POINTER(class_name); \
  SL_DECLARE_DYNAMIC_CLASS(class_name, id)


///////////////////////////////////////////////////////////////////////////////
//
// Put this in your abstract visitor's implementation file.
//
///////////////////////////////////////////////////////////////////////////////

#define SG_IMPLEMENT_VISITOR(class_name, base_class_name) \
  SL_IMPLEMENT_CLASS(class_name, base_class_name)


///////////////////////////////////////////////////////////////////////////////
//
// Put this in your concrete visitor's implementation file.
//
///////////////////////////////////////////////////////////////////////////////

#define SG_IMPLEMENT_DYNAMIC_VISITOR(class_name, base_class_name) \
  SL_IMPLEMENT_DYNAMIC_CLASS(class_name, base_class_name)


// Handy macro for declaring all the visitor functions.
// Use in classes that inherit from this one.
#define SG_DECLARE_ALL_VISITOR_FUNCTIONS \
  virtual bool          visit ( SgBackground & ); \
  virtual bool          visit ( SgBaseColor & ); \
  virtual bool          visit ( SgBillboard & ); \
  virtual bool          visit ( SgCamera & ); \
  virtual bool          visit ( SgCircle & ); \
  virtual bool          visit ( SgColorWheel & ); \
  virtual bool          visit ( SgCone & ); \
  virtual bool          visit ( SgCoordinate3 & ); \
  virtual bool          visit ( SgCoordinate4 & ); \
  virtual bool          visit ( SgCube & ); \
  virtual bool          visit ( SgCylinder & ); \
  virtual bool          visit ( SgDirectionalLight & ); \
  virtual bool          visit ( SgDrawStyle & ); \
  virtual bool          visit ( SgEllipse & ); \
  virtual bool          visit ( SgGroup & ); \
  virtual bool          visit ( SgIdentity & ); \
  virtual bool          visit ( SgLineSet & ); \
  virtual bool          visit ( SgMaterial & ); \
  virtual bool          visit ( SgNode & ); \
  virtual bool          visit ( SgNormal & ); \
  virtual bool          visit ( SgOrthographicCamera & ); \
  virtual bool          visit ( SgPerspectiveCamera & ); \
  virtual bool          visit ( SgPointSet & ); \
  virtual bool          visit ( SgQuadMesh & ); \
  virtual bool          visit ( SgRectangle & ); \
  virtual bool          visit ( SgRotation & ); \
  virtual bool          visit ( SgScale & ); \
  virtual bool          visit ( SgSelection & ); \
  virtual bool          visit ( SgSeparator & ); \
  virtual bool          visit ( SgShading & ); \
  virtual bool          visit ( SgSphere & ); \
  virtual bool          visit ( SgSquare & ); \
  virtual bool          visit ( SgSwitch & ); \
  virtual bool          visit ( SgTransform & ); \
  virtual bool          visit ( SgTranslation & ); \
  virtual bool          visit ( SgTriangle & ); \
  virtual bool          visit ( SgTriStrip & ); \
  virtual bool          visit ( SgUnScale & ); \
  virtual bool          visit ( SgShape & ); \
  virtual bool          visit ( SgCoordinate & ); \
  virtual bool          visit ( SgLight & ); \
  virtual bool          visit ( SgVertexSet & ); \
  virtual bool          visit ( SgPrimitive & ); \
  virtual bool          visit ( SgText & )


///////////////////////////////////////////////////////////////////////////////
//
// Handy macro for defining all the node classes.
//
///////////////////////////////////////////////////////////////////////////////

#define SG_FORWARD_DECLARE_ALL_NODES \
namespace CadKit \
{ \
  class SgBackground; \
  class SgBaseColor; \
  class SgBillboard; \
  class SgCamera; \
  class SgCircle; \
  class SgColorWheel; \
  class SgCone; \
  class SgCoordinate; \
  class SgCoordinate3; \
  class SgCoordinate4; \
  class SgCube; \
  class SgCylinder; \
  class SgDefine; \
  class SgDirectionalLight; \
  class SgDrawStyle; \
  class SgEllipse; \
  class SgGroup; \
  class SgIdentity; \
  class SgLineSet; \
  class SgMaterial; \
  class SgNode; \
  class SgNormal; \
  class SgOrthographicCamera; \
  class SgPerspectiveCamera; \
  class SgPointSet; \
  class SgQuadMesh; \
  class SgRectangle; \
  class SgRotation; \
  class SgScale; \
  class SgSelection; \
  class SgSeparator; \
  class SgShading; \
  class SgSphere; \
  class SgSquare; \
  class SgSwitch; \
  class SgTransform; \
  class SgTranslation; \
  class SgTriStrip; \
  class SgTriangle; \
  class SgUnScale; \
  class SgShape; \
  class SgCoordinate; \
  class SgLight; \
  class SgVertexSet; \
  class SgPrimitive; \
  class SgText; \
}


#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_VISITOR_MACROS_H_
