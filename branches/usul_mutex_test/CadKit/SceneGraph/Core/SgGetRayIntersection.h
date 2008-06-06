
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
# define CADKIT_DEFINE_SL_LINE_3_MATRIX_FUNCTIONS
# include "Standard/SlLine3.h"
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
  SlMatrix44f _M;
  SlMatrix44f _IM;

  SG_DECLARE_DYNAMIC_VISITOR ( SgGetRayIntersection, 0x00001015 );
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_GET_RAY_INTERSECTION_RENDERER_H_
