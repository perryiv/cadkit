
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgDraw.cpp: Misc draw functions.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgDraw.h"
#include "SgSeparator.h"
#include "SgCoordinate3.h"
#include "SgLineSet.h"
#include "SgPointSet.h"
#include "SgPrimitive.h"
#include "SgTranslation.h"
#include "SgUnScale.h"
#include "SgScale.h"

using namespace CadKit;


/////////////////////////////////////////////////////////////////////////////
//
//  Draw the line.
//
/////////////////////////////////////////////////////////////////////////////

SgSeparator *SgDraw::drawLine ( const SlVec3f &pt0, const SlVec3f &pt1 )
{
  return drawLine (
    (float) pt0[0],
    (float) pt0[1], 
    (float) pt0[2], 
    (float) pt1[0], 
    (float) pt1[1], 
    (float) pt1[2] );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Draw the line.
//
/////////////////////////////////////////////////////////////////////////////

SgSeparator *SgDraw::drawLine ( const SlVec2f &pt0, const SlVec2f &pt1 )
{
  return drawLine (
    (float) pt0[0],
    (float) pt0[1], 
    0.0f, 
    (float) pt1[0], 
    (float) pt1[1], 
    0.0f );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Draw the line.
//
/////////////////////////////////////////////////////////////////////////////

SgSeparator *SgDraw::drawLine ( const float &x0, const float &y0, const float &x1, const float &y1 )
{
  return drawLine ( 
    x0, 
    y0, 
    0.0f, 
    x1, 
    y1, 
    0.0f );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Draw the line.
//
/////////////////////////////////////////////////////////////////////////////

SgSeparator *SgDraw::drawLine ( const float &x0, const float &y0, const float &z0, const float &x1, const float &y1, const float &z1 )
{
  SgCoordinate3 *coords = new SgCoordinate3;
  SL_ASSERT ( coords );
  if ( !coords ) 
    return NULL;

  SgLineSet *lineSet = new SgLineSet;
  SL_ASSERT ( lineSet );
  if ( !lineSet ) 
    return NULL;

  SgSeparator *branch = new SgSeparator;
  SL_ASSERT ( branch );
  if ( !branch ) 
    return NULL;

  coords->setNumPoints ( 2 );
  coords->setPoint ( x0, y0, z0, 0 );
  coords->setPoint ( x1, y1, z1, 1 );

  branch->addChild ( coords );
  branch->addChild ( lineSet );

  return branch;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Draw the line.
//
/////////////////////////////////////////////////////////////////////////////

SgSeparator *SgDraw::drawLine ( const SlVec3d &pt0, const SlVec3d &pt1 )
{
  return drawLine (
    (float) pt0[0],
    (float) pt0[1], 
    (float) pt0[2], 
    (float) pt1[0], 
    (float) pt1[1], 
    (float) pt1[2] );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Draw the line.
//
/////////////////////////////////////////////////////////////////////////////

SgSeparator *SgDraw::drawLine ( const SlVec2d &pt0, const SlVec2d &pt1 )
{
  return drawLine (
    (float) pt0[0],
    (float) pt0[1], 
    0.0f, 
    (float) pt1[0], 
    (float) pt1[1], 
    0.0f );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Draw the line.
//
/////////////////////////////////////////////////////////////////////////////

SgSeparator *SgDraw::drawLine ( const double &x0, const double &y0, const double &x1, const double &y1 )
{
  return drawLine (
    (float) x0, 
    (float) y0, 
    0.0f, 
    (float) x1, 
    (float) y1, 
    0.0f );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Draw the line.
//
/////////////////////////////////////////////////////////////////////////////

SgSeparator *SgDraw::drawLine ( const double &x0, const double &y0, const double &z0, const double &x1, const double &y1, const double &z1 )
{
  return drawLine (
    (float) x0, 
    (float) y0, 
    (float) z0, 
    (float) x1, 
    (float) y1, 
    (float) z1 );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Draw the 2D points.
//
/////////////////////////////////////////////////////////////////////////////

bool SgDraw::draw2dPoints (
  const double *points, 
  const unsigned long &numPoints, 
  const SlType *type, 
  const float &size, 
  bool setScale,
  bool uniformScale,
  bool willRotate,
  const float &z,
  SgGroup *branch )
{
  SL_ASSERT ( points );
  SL_ASSERT ( type && type->isDerivedFrom ( SgPrimitive::getClassType() ) );
  SL_ASSERT ( size > 0.0f );
  SL_ASSERT ( branch );

  // Use the same object over and over.
  SgPrimitive *prim = (SgPrimitive *) type->createObject();
  if ( !prim ) 
    return false;

  SgDraw::_setCenter ( prim, 0.0f, 0.0f, 0.0f );

  SgSeparator *subBranch = NULL;
  SgTranslation *translation = NULL;
  
  SgUnScale *unScale = NULL;
  SgScale *scale = NULL;

  if ( setScale )
  {
    unScale = new SgUnScale;
    if ( !unScale ) 
      return false;

    scale = new SgScale ( SlVec3f ( size, size, size ) );
    if ( !scale ) 
      return false;

    if ( willRotate )   unScale->addFlags    ( SgUnScale::PROCESS_ROTATIONS );
    else                unScale->removeFlags ( SgUnScale::PROCESS_ROTATIONS );

    if ( uniformScale ) unScale->addFlags    ( SgUnScale::ASSUME_UNIFORM_SCALE );
    else                unScale->removeFlags ( SgUnScale::ASSUME_UNIFORM_SCALE );
  }

  else SgDraw::_setSize ( prim, size );

  // Shortcuts.
  const double *x = points;
  const double *y = &x[numPoints];

  for ( unsigned long i = 0; i < numPoints; ++i )
  {
    subBranch = new SgSeparator;
    if ( !subBranch ) 
      return false;
    branch->addChild ( subBranch );

    translation = new SgTranslation;
    if ( !translation ) 
      return false;

    translation->setTranslation ( SlVec3f ( (float) x[i], (float) y[i], z ) );
    subBranch->addChild ( translation );

    if ( setScale ) 
    {
      subBranch->addChild ( unScale );
      subBranch->addChild ( scale );
    }

    subBranch->addChild ( prim );
  }

  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Draw the points as pixels.
//
/////////////////////////////////////////////////////////////////////////////

bool SgDraw::drawPointsAsPixels (
  const double *points, 
  const unsigned long &numPts, 
  const unsigned long &dimension,
  SgGroup *branch )
{
  SL_ASSERT ( points );
  SL_ASSERT ( numPts > 0 );
  SL_ASSERT ( dimension >= 2 );
  SL_ASSERT ( branch );

  SgCoordinate3 *coords = new SgCoordinate3;
  SL_ASSERT ( coords );
  if ( !coords ) 
    return false;

  SgPointSet *pointSet = new SgPointSet;
  SL_ASSERT ( pointSet );
  if ( !pointSet ) 
    return false;

  coords->setNumPoints ( numPts );
  const double *x = points;
  const double *y = &x[numPts];

  if ( dimension == 2 )
  {
    for ( unsigned long i = 0; i < numPts; ++i )
    {
      coords->setPoint ( (float) x[i], (float) y[i], 0.0f, i );
    }
  }

  else
  {
    const double *z = &y[numPts];

    for ( unsigned long i = 0; i < numPts; ++i )
    {
      coords->setPoint ( (float) x[i], (float) y[i], (float) z[i], i );
    }
  }

  branch->addChild ( coords );
  branch->addChild ( pointSet );

  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the center of the primitive.
//
/////////////////////////////////////////////////////////////////////////////

void SgDraw::_setCenter ( SgPrimitive *prim, const float &x, const float &y, const float &z )
{
  SL_ASSERT ( 0 ); // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the size of the primitive.
//
/////////////////////////////////////////////////////////////////////////////

void SgDraw::_setSize ( SgPrimitive *prim, const float &size )
{
  SL_ASSERT ( 0 ); // TODO.
}
