
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
  virtual bool          visit ( SgPrimitive & )


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
}


#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_VISITOR_MACROS_H_
