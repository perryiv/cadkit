
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

#include "SgPrecompiled.h"
#include "SgGetRayIntersection.h"
#include "SgGroup.h"
#include "SgSphere.h"
#include "SgTranslation.h"
#include "SgCube.h"
#include "SgPerspectiveCamera.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
# include "Standard/SlBoundingBox.h"
#endif

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_VISITOR(SgGetRayIntersection,SgVisitor);


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgGetRayIntersection::SgGetRayIntersection() : SgVisitor(),
  _node ( NULL ),
  _global ( 0.0f, 0.0f, 0.0f ),
  _local ( 0.0f, 0.0f, 0.0f ),
  _ray ( SlVec3f ( 0.0f, 0.0f, 0.0f ), SlVec3f ( 1.0f, 0.0f, 0.0f ) ),
  _M ( SL_MATRIX4_IDENTITY_F ),
  _IM ( SL_MATRIX4_IDENTITY_F )
{
  SL_PRINT2 ( "SgGetRayIntersection::SgGetRayIntersection(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgGetRayIntersection::~SgGetRayIntersection()
{
  SL_PRINT2 ( "SgGetRayIntersection::~SgGetRayIntersection(), this = %X\n", this );
  // Nothing to delete.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the ray to intersect with.
//
/////////////////////////////////////////////////////////////////////////////

void SgGetRayIntersection::setRay ( const SlLine3f &ray )
{
  SL_ASSERT ( this );
  SL_PRINT2 ( "SgGetRayIntersection::setRay(), this = %X\n", this );
  _ray.setValue ( ray );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgBackground & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgBaseColor & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgBillboard & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgCamera & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgCircle & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgColorWheel & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgCone & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgCoordinate3 & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgCoordinate4 & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgCube &cube )
{
  SL_ASSERT ( this );

  // Transform a copy of the ray to local space.
  SlLine3f ray = _ray;
  ray.transform ( _IM );
  ray.normalize();

  // Calculate the bounding box.
  float halfSize = cube.size * 0.5f;
  const SlVec3f &center = cube.center;
  SlVec3f min ( center[0] - halfSize, center[1] - halfSize, center[2] - halfSize );
  SlVec3f max ( center[0] + halfSize, center[1] + halfSize, center[2] + halfSize );
  SlBoundingBoxf bbox ( min, max );

  // Intersect with the cube's bounding box.
  bool result = bbox.intersect ( ray.getPoint(), ray.getDirection(), _local );
  _node = ( result ) ? ( &cube ) : NULL;

  // If we hit it...
  if ( _node.isValid() ) 
  {
    // Transform the local intersection to global space.
    _global = _M * _local;

    // If the global point is on the wrong side of the near clipping plane 
    // then we don't have an intersection.
    if ( _global[2] >= _ray.getPoint()[2] ) 
      _node = NULL;
  }

  // We did our job.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgCylinder & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgDirectionalLight & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgDrawStyle & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgEllipse & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgGroup &group )
{
  SL_ASSERT ( this );
  SgNode::Ptr bestNode ( NULL );
  SlVec3f local, global;
  float minDist, dist;
  bool first = true;

  // Get the number of children.
  unsigned int numChildren = group.getNumChildren();

  // Loop through all the nodes.
  for ( unsigned int i = 0; i < numChildren; ++i )
  {
    // Initialize.
    _node = NULL;

    // Give the node this visitor.
    if ( !group.getChild ( i )->accept ( *this ) ) 
    {
      SL_ASSERT ( 0 ); // Why didn't it work?
      return false;
    }

    // If we hit a node...
    if ( _node.isValid() )
    {
      // Get the distance from the ray's start to the intersected point.
      dist = _global.getDistanceSquared ( _ray.getPoint() );

      // The first time we do this.
      if ( first )
      {
        first = false;
        minDist = dist;
        global = _global;
        local = _local;
        bestNode = _node;
      }

      // Every time after we do this.
      else if ( dist < minDist )
      {
        minDist = dist;
        global = _global;
        local = _local;
        bestNode = _node;
      }
    }
  }

  // We we're done looping set the best node (which may still be NULL).
  _node = bestNode;

  // If we have a node then set the intersection points.
  if ( _node.isValid() )
  {
    _global = global;
    _local = local;
  }

  // If we get here then it worked.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgIdentity & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgLineSet & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgMaterial &material )
{
  SL_ASSERT ( this );
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgNormal & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgOrthographicCamera & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgPerspectiveCamera &camera )
{
  SL_ASSERT ( this );

  // We don't have to worry about the projection matrix.
  _M.setValue ( camera.getModelviewMatrix() );

  // Get the inverse now too.
  _IM.setValue ( _M );
  if ( !_IM.invert() ) 
    return false;

  // It worked.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgPointSet & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgQuadMesh & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgRectangle & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgRotation & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgScale & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgSelection &selection )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgSeparator &separator )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgShading & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgSphere &sphere )
{
  SL_ASSERT ( this );

  // Transform a copy of the ray to local space.
  SlLine3f ray = _ray;
  ray.transform ( _IM );
  ray.normalize();

  // Intersect with the sphere.
  bool result = CadKit::intersectLineAndSphere ( 
    ray.getPoint(), 
    ray.getDirection(), 
    sphere.getCenter(), 
    sphere.getRadius(),
    _local );

  // Assign the node.
  _node = ( result ) ? ( &sphere ) : NULL;

  // If we hit the sphere then transform the local intersection to global space.
  if ( _node.isValid() ) 
    _global = _M * _local;

  // If the global point is on the wrong side of the near clipping plane 
  // then we don't have an intersection.
  if ( _global[2] >= ray.getPoint()[2] ) 
    _node = NULL;

  // We did our job.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgSquare & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgSwitch & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgTransform & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgTranslation &t )
{
  SL_ASSERT ( this );

  // Modify the modelview.
  _M.translate ( t.getTranslation() );

  // Get the inverse of the modelview matrix.
  if ( !_IM.invert() ) 
    return false;

  // We did our job.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgTriangle & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgTriStrip & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgUnScale & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgShape & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgCoordinate & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgLight & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgVertexSet & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgPrimitive & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGetRayIntersection::visit ( SgNode & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}
