
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
//  SgVisitor: The base visitor class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_BASE_VISITOR_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_BASE_VISITOR_H_

#include "SgApi.h"
#include "SgTypedefs.h"
#include "SgVisitorMacros.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlRefBase.h"
# include <list>
#endif

// Declare all the nodes for this class.
SG_FORWARD_DECLARE_ALL_NODES;


namespace CadKit
{
class SG_API SgVisitor : public SlRefBase
{
public:

  // The visit functions.
  virtual bool          visit ( SgBackground & ) = 0;
  virtual bool          visit ( SgBaseColor & ) = 0;
  virtual bool          visit ( SgBillboard & ) = 0;
  virtual bool          visit ( SgCamera & ) = 0;
  virtual bool          visit ( SgCircle & ) = 0;
  virtual bool          visit ( SgColorWheel & ) = 0;
  virtual bool          visit ( SgCone & ) = 0;
  virtual bool          visit ( SgCoordinate3 & ) = 0;
  virtual bool          visit ( SgCoordinate4 & ) = 0;
  virtual bool          visit ( SgCube & ) = 0;
  virtual bool          visit ( SgCylinder & ) = 0;
  virtual bool          visit ( SgDirectionalLight & ) = 0;
  virtual bool          visit ( SgDrawStyle & ) = 0;
  virtual bool          visit ( SgEllipse & ) = 0;
  virtual bool          visit ( SgGroup & ) = 0;
  virtual bool          visit ( SgIdentity & ) = 0;
  virtual bool          visit ( SgLineSet & ) = 0;
  virtual bool          visit ( SgMaterial & ) = 0;
  virtual bool          visit ( SgNode & ) = 0;
  virtual bool          visit ( SgNormal & ) = 0;
  virtual bool          visit ( SgOrthographicCamera & ) = 0;
  virtual bool          visit ( SgPerspectiveCamera & ) = 0;
  virtual bool          visit ( SgPointSet & ) = 0;
  virtual bool          visit ( SgQuadMesh & ) = 0;
  virtual bool          visit ( SgRectangle & ) = 0;
  virtual bool          visit ( SgRotation & ) = 0;
  virtual bool          visit ( SgScale & ) = 0;
  virtual bool          visit ( SgSelection & ) = 0;
  virtual bool          visit ( SgSeparator & ) = 0;
  virtual bool          visit ( SgShading & ) = 0;
  virtual bool          visit ( SgSphere & ) = 0;
  virtual bool          visit ( SgSquare & ) = 0;
  virtual bool          visit ( SgSwitch & ) = 0;
  virtual bool          visit ( SgTransform & ) = 0;
  virtual bool          visit ( SgTranslation & ) = 0;
  virtual bool          visit ( SgTriangle & ) = 0;
  virtual bool          visit ( SgTriStrip & ) = 0;
  virtual bool          visit ( SgUnScale & ) = 0;
  virtual bool          visit ( SgShape & ) = 0;
  virtual bool          visit ( SgCoordinate & ) = 0;
  virtual bool          visit ( SgLight & ) = 0;
  virtual bool          visit ( SgVertexSet & ) = 0;
  virtual bool          visit ( SgPrimitive & ) = 0;

  // Push/pop the node. These are called from each node's accept() function.
  // Normal usage does not require calling these, but they have to be public.
  void                  pushNode ( SgNode * );
  void                  popNode();

  // Reset the visitor. Call this in between scene visits to ensure that
  // any accumulated states are reset.
  virtual bool          reset();

  // Reference/unreference the visitor.
  void                  ref()   { this->_incrementReferenceCount(); }
  void                  unref() { this->_decrementReferenceCount(); }

protected:

  typedef std::list<SgNode *> Stack; // Compiler doesn't like SlRefPtr here.
  Stack _stack;

  SgVisitor();
  virtual ~SgVisitor();

  SG_DECLARE_VISITOR ( SgVisitor, 0x00001039 );
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_BASE_VISITOR_H_
