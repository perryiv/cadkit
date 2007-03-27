
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlGeometry.h: Some functions for geometry calculations.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TEMPLATE_GEOMETRY_H_
#define _CADKIT_STANDARD_LIBRARY_TEMPLATE_GEOMETRY_H_

#include "SlVec3.h"


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Intersect the line with the plane. Return false if parallel.
//  Adopted from "computePlaneEq()" found in planeSets.c in Gems III.
//
///////////////////////////////////////////////////////////////////////////////

template<class Vec3, class Real> bool intersectLineAndPlane ( 
  const Vec3 &linePoint,   // A point on the line.
  const Vec3 &lineVec,     // The line's (normalized) direction vector.
  const Vec3 &planeNormal, // The (normalized) normal vector of the plane.
  const Real &planeDist,   // Distance to the origin.
  Vec3 &answer )           // The intersection point.
{
  // We need to see if this is zero.
  Real denom ( planeNormal.dot ( lineVec ) );

  // If the dot product of the plane's normal and the line's direction 
  // (see above) is zero then the line is parallel to the plane
  // (i.e., no intersection).
  if ( denom == 0.0f ) 
    return false;

  // Compute the intersection point.
  Real tt = - ( planeNormal.dot ( linePoint ) + planeDist ) / denom;
  answer[0] = linePoint[0] + ( tt * lineVec[0] );
  answer[1] = linePoint[1] + ( tt * lineVec[1] );
  answer[2] = linePoint[2] + ( tt * lineVec[2] );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect the line with the plane. Return false if parallel.
//  This algorithm is from the function "computePlaneEq()" found in file 
//  planeSets.c in Gems III. That function has to find the "point", here 
//  it is passed in.
//
///////////////////////////////////////////////////////////////////////////////

template<class Vec3> bool intersectLineAndPlane ( 
  const Vec3 &linePoint,   // A point on the line.
  const Vec3 &lineVec,     // The line's (normalized) direction vector.
  const Vec3 &planePoint,  // A point on the plane.
  const Vec3 &planeNormal, // The (normalized) normal vector of the plane.
  Vec3 &answer )           // The intersection point.
{
  // Call the other function.
  return CadKit::intersectLineAndPlane (
    linePoint, 
    lineVec, 
    planeNormal,
    // Calculate the distance from the plane to the origin (which could be
    // negative). Cannot catch in local variable because all compilers do 
    // not agree on how to do this (g++ doesn't like Vec::Real).
    - planePoint.dot ( planeNormal ),
    answer
    );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect the line with the sphere.
//
///////////////////////////////////////////////////////////////////////////////

template<class Vec3, class Real> bool intersectLineAndSphere ( 
  const Vec3 &E, // A point on the line.
  const Vec3 &V, // The line's (normalized) direction vector.
  const Vec3 &O, // Center of the sphere.
  const Real &r, // Sphere's radius.
  Vec3 &answer ) // The intersection point.
{
  // The vector between the sphere's center and the line's reference point.
	Vec3 EO = O - E;

  // ?
	Real v = EO.dot ( V );
  Real dp = EO.dot ( EO );
	Real disc = r * r - dp - v * v;

  // If the disc is less than zero then there is no intersection.
	if ( disc < 0 ) 
    return false;

  // ?
  Real d = CadKit::getSquareRoot ( disc );
	answer = E + ( v - d ) * V;

  // It worked.
	return true;
}


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_TEMPLATE_GEOMETRY_H_
