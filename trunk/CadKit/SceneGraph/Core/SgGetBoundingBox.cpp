
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgGetBoundingBox: The bounding box class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgGetBoundingBox.h"
#include "SgGroup.h"
#include "SgSphere.h"
#include "SgRotation.h"
#include "SgScale.h"
#include "SgTransform.h"
#include "SgTranslation.h"
#include "SgCube.h"

#include "Standard/SlPrint.h"
#include "Standard/SlTrace.h"
#include "Standard/SlConstants.h"

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_VISITOR(SgGetBoundingBox,SgVisitor);


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGetBoundingBox::SgGetBoundingBox() : SgVisitor(),
  _M ( SL_MATRIX4_IDENTITY_F )
{
  SL_PRINT2 ( "SgGetBoundingBox::SgGetBoundingBox(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGetBoundingBox::~SgGetBoundingBox()
{
  SL_PRINT2 ( "SgGetBoundingBox::~SgGetBoundingBox(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgBackground & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgBaseColor & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgBillboard & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgCamera & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgCircle & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgColorWheel & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgCone & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgCoordinate3 & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgCoordinate4 & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgCube &cube )
{
  SL_ASSERT ( this );

  // Calculate the local bounding box.
  // Note: g++ does not like the bbox constructor arguments to be vector 
  // constructor calls, which is why it is broken into these steps.
  const SlVec3f &c = cube.center;
  SlFloat32 hs = cube.size * 0.5f;
  SlVec3f bbmin ( c[0] - hs, c[1] - hs, c[2] - hs );
  SlVec3f bbmax ( c[0] + hs, c[1] + hs, c[2] + hs );
  SlBoundingBoxf bbox ( bbmin, bbmax );
  SL_TRACE7 ( "       bbox = %12.7f%12.7f%12.7f%12.7f%12.7f%12.7f\n", bbox.getMin()[0], bbox.getMin()[1], bbox.getMin()[2], bbox.getMax()[0], bbox.getMax()[1], bbox.getMax()[2] );

  // Put it in model space.
  bbox = _M * bbox;
  SL_TRACE17 ( "_M = \n%12.7f%12.7f%12.7f%12.7f\n%12.7f%12.7f%12.7f%12.7f\n%12.7f%12.7f%12.7f%12.7f\n%12.7f%12.7f%12.7f%12.7f\n", _M[0], _M[4], _M[8], _M[12], _M[1], _M[5], _M[9], _M[13], _M[2], _M[6], _M[10], _M[14], _M[3], _M[7], _M[11], _M[15] );

  // Add it to the sum.
  _bbox += bbox;
  SL_TRACE7 ( "      _bbox = %12.7f%12.7f%12.7f%12.7f%12.7f%12.7f\n", _bbox.getMin()[0], _bbox.getMin()[1], _bbox.getMin()[2], _bbox.getMax()[0], _bbox.getMax()[1], _bbox.getMax()[2] );

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgCylinder & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgDirectionalLight & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgDrawStyle & )
{
  SL_ASSERT ( this );
  return true; // No bounding box.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgEllipse & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgGroup &group )
{
  SL_ASSERT ( this );

  // Get the number of children.
  unsigned int numChildren = group.getNumChildren();

  // Initialize the return value.
  bool result ( true );

  // Loop through all the nodes.
  for ( unsigned int i = 0; i < numChildren; ++i )
  {
    // Give the node this visitor.
    if ( !group.getChild ( i )->accept ( *this ) ) 
    {
      SL_ASSERT ( 0 ); // Why didn't it work?
      SL_TRACE2 ( "In SgGetBoundingBox::visit ( SgGroup & ), failed to get bounding box of child %d", i );

      // Set the result but keep going.
      result = false;
    }
  }

  // Return the result.
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgIdentity & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgLineSet & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgMaterial & )
{
  SL_ASSERT ( this );
  return true; // No bounding box.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgNormal & )
{
  SL_ASSERT ( this );
  return true; // No bounding box.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgOrthographicCamera & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgPerspectiveCamera &camera )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgPointSet & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgQuadMesh & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgRectangle & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgRotation &r )
{
  SL_ASSERT ( this );
  _M.rotate ( SL_DEG_TO_RAD_F * r.getDegrees(), r.getAxis() ); // Modify the modelview matrix.
  return true; // No bounding box.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgScale &scale )
{
  SL_ASSERT ( this );
  const SlVec3f &s = scale.getScale();
  _M.scale ( SlVec3f ( s[0], s[1], s[2] ) );
  return true; // No bounding box.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgSelection &selection )
{
  SL_ASSERT ( this );
  return this->visit ( (SgSeparator &) selection );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgSeparator &separator )
{
  SL_ASSERT ( this );
  return this->visit ( (SgGroup &) separator );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgShading & )
{
  SL_ASSERT ( this );
  return true; // No bounding box.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgSphere &sphere )
{
  SL_ASSERT ( this );
  SL_ASSERT ( sphere.getRadius() > 0.0 );

  // Calculate the local bounding box.
  // Note: g++ needs you to declare bbmin and bbmax separately, rather
  // than put the vector constructor calls as arguments in the bbox 
  // constructor call.
  const SlVec3f &c = sphere.getCenter();
  const SlFloat32 &r = sphere.getRadius();
  SlVec3f bbmin ( c[0] - r, c[1] - r, c[2] - r );
  SlVec3f bbmax ( c[0] + r, c[1] + r, c[2] + r );
  SlBoundingBoxf bbox ( bbmin, bbmax );

  // Put it in model space.
  bbox = _M * bbox;

  // Add it to the sum.
  _bbox += bbox;

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgSquare & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgSwitch & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgTransform &t )
{
  SL_ASSERT ( this );
  _M.multRight ( t.getMatrix() ); // Modify the modelview matrix.
  return true; // No bounding box.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgTranslation &t )
{
  SL_ASSERT ( this );
  _M.translate ( t.getTranslation() ); // Modify the modelview matrix.
  return true; // No bounding box.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgTriangle & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgTriStrip & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgUnScale & )
{
  SL_ASSERT ( this );
  return true; // No bounding box.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgShape & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgCoordinate & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgLight & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgVertexSet & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgPrimitive & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetBoundingBox::visit ( SgNode & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}
