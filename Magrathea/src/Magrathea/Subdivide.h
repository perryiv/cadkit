
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAGRATHEA_SUBDIVIDE_H__
#define __MAGRATHEA_SUBDIVIDE_H__

#include "Usul/Math/Absolute.h"
#include "Usul/Math/Vector2.h"

#include <algorithm>

namespace Magrathea
{
  namespace Detail
  {

    template < typename Point >
    struct Triangle
    {
      typedef typename Point PointType;

      Point p0;
      Point p1;
      Point p2;

      Triangle( const Point& point0, const Point& point1, const Point& point2 ) :
      p0 ( point0 ),
      p1 ( point1 ),
      p2 ( point2 )
      {
      }

      Point center() const
      {
        Usul::Math::Vec2d midPoint;
        midPoint[0] = ( p0.latd() + p1.latd() ) / 2.0;
        midPoint[1] = ( p0.lond() + p1.lond() ) / 2.0;

        Point center;
        center.latd ( ( midPoint[0] + p2.latd() ) / 2.0 );
        center.lond ( ( midPoint[1] + p2.lond() ) / 2.0 );
        center.height ( ( p0.height() + p1.height() + p2.height() ) / 3.0 );

        return center;
      }
    };
  }

  template < typename Triangle, typename Triangles >
  bool trisect ( const Triangle& triangle, Triangles& triangles, unsigned int &depth )
  {
    //unsigned int maximumDepth ( ::pow ( 3.0, 3 ) );
    unsigned int maximumDepth ( 2 );
    if( depth > maximumDepth )
      return true;

    // Get the center of the triangle.
    typedef typename Triangle::PointType Point;
    Point center ( triangle.center() );

    // Save the height.
    double oldHeight ( center.height() );

    // Calculate the real height
    double deltaH(  ossimElevManager::instance()->getHeightAboveMSL( center ) );
    if(deltaH == OSSIM_DBL_NAN)
    {
      deltaH = 0.0;
    }

    center.height( deltaH + ossimGeoidManager::instance()->offsetFromEllipsoid( center ) );

    /// fifty cm resolution
    double errorFactor ( 0.5 );

    if( Usul::Math::absolute ( center.height() - oldHeight ) < errorFactor )
      return true;

    /// Make 3 new triangles
    Triangle t0 ( triangle.p0, triangle.p2, center );
    Triangle t1 ( triangle.p0, center, triangle.p1 );
    Triangle t2 ( triangle.p1, center, triangle.p2 );

    ++depth;
    {
      unsigned int currentDepth ( depth );
      if ( trisect ( t0, triangles, currentDepth ) )
        triangles.push_back ( t0 );
    }

    {
      unsigned int currentDepth ( depth );
      if ( trisect ( t1, triangles, currentDepth ) )
        triangles.push_back ( t1 );
    }

    {
      unsigned int currentDepth ( depth );
      if ( trisect ( t2, triangles, currentDepth ) )
        triangles.push_back ( t2 );
    }
    return false;
  }

  template < typename Points >
  void subdivide ( const Points& input, Points& output )
  {
    // Return if no input.
    if( input.empty() )
      return;

    // Throw if we don't have the right number of points.
    if ( input.size() % 3 != 0 )
      throw std::invalid_argument( "Error 641631206: Number of points are not divisible by three" );

    typedef Detail::Triangle < Points::value_type > Triangle;
    typedef std::list < Triangle > Triangles;
    Triangles triangles;

    // Create the triangles.
    for ( typename Points::const_iterator iter = input.begin(); iter != input.end(); iter += 3 )
    {
      Triangle triangle ( *iter, *(iter + 1 ), *(iter + 2 ) );
      
      unsigned int depth ( 0 );
      if ( trisect ( triangle, triangles, depth ) )
        triangles.push_back ( triangle );
    }

    output.reserve ( triangles.size() * 3 );

    // Copy the triangles into the output.
    for( Triangles::const_iterator iter = triangles.begin(); iter != triangles.end(); ++iter )
    {
      output.push_back( iter->p0 );
      output.push_back( iter->p1 );
      output.push_back( iter->p2 );
    }
  }
}

#endif 
