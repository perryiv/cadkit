
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
//  SgGetRayIntersection: The ray intersection class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_GET_RAY_INTERSECTION_RENDERER_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_GET_RAY_INTERSECTION_RENDERER_H_

#include "SgVisitor.h"
#include "SgNode.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlVec3.h"
# include "Standard/SlLine3.h"
# include "Standard/SlMatrix4.h"
#endif


namespace CadKit
{
class SG_API SgGetRayIntersection : public SgVisitor
{
public:

  SgGetRayIntersection();
  virtual ~SgGetRayIntersection();

  // Get the intersected node (if any).
  SgNode *             getIntersectedNode() const { return _node.getValue(); }

  // Get the global/local intersection point. If there is no intersected 
  // node, then these points are not set.
  const SlVec3f &      getGlobalIntersectionPoint() const { return _global; }
  const SlVec3f &      getLocalIntersectionPoint() const { return _local; }

  // Get/set the ray to intersect with.
  const SlLine3f &     getRay() const { return _ray; }
  void                 setRay ( const SlLine3f &ray );

  // The visit functions.
  SG_DECLARE_ALL_VISITOR_FUNCTIONS;

protected:

  SgNode::Ptr _node;
  SlVec3f _global;
  SlVec3f _local;
  SlLine3f _ray;
  SlMatrix4f _M;
  SlMatrix4f _IM;

  SG_DECLARE_DYNAMIC_VISITOR ( SgGetRayIntersection, 0x00001015 );
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_GET_RAY_INTERSECTION_RENDERER_H_
