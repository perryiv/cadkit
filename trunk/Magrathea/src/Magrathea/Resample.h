
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAGRATHEA_RESAMPLE_H__
#define __MAGRATHEA_RESAMPLE_H__

#include "Usul/Math/Absolute.h"

namespace Magrathea
{
  template < typename Point >
  struct ParametricPoint
  {
    typedef Point PointType;

    Point p;
    double u;

    bool operator< ( const ParametricPoint& rhs ) const
    {
      return this->u < rhs.u;
    }
  };



  template < typename Point, typename ParmetricPoints >
  void bisect ( Point &point0, Point &point2, ParmetricPoints& answer )
  {
    double u1 ( ( point0.u + point2.u ) * 0.50 );

    double lat1 ( ( point0.p.latd() + point2.p.latd() ) * 0.50 );
    double lon1 ( ( point0.p.lond() + point2.p.lond() ) * 0.50 );
    double height1 ( ( point0.p.height() + point2.p.height() ) * 0.50 );

    // Calculate the height of the mid-point.
    typename Point::PointType point1 ( lat1, lon1 );
    double deltaH(  ossimElevManager::instance()->getHeightAboveMSL( point1 ) );
    if(deltaH == OSSIM_DBL_NAN)
    {
      deltaH = 0.0;
    }

    point1.height( deltaH + ossimGeoidManager::instance()->offsetFromEllipsoid( point1 ) );

    //double distance02 ( ( point2.p.latd() - point0.p.latd() ) * ( point2.p.latd() - point0.p.latd() )
    //  + ( ( point2.p.lond() - point0.p.lond() ) * ( point2.p.lond() - point0.p.lond() ) ) );

    //double errorFactor ( distance02 * 0.1 );

    /// one cm resolution
    double errorFactor ( 0.01 );

    //double distance0 ( point1.distanceTo( point0.p ) );
    //double distance2 ( point1.distanceTo( point2.p ) );

    if( Usul::Math::absolute ( point1.height() - height1 ) /*< errorFactor && distance0 < 2.0 && distance2 < 2.0*/ )
      return;

    Point midPoint;
    midPoint.p = point1;
    midPoint.u = u1;

    answer.push_back ( midPoint );

    bisect ( point0, midPoint, answer );
    bisect ( midPoint, point2, answer );
  }

  template < typename Points >
  void resample ( const Points& input, Points& output )
  {
    // Return if no input.
    if( input.empty() )
      return;

    // Set the output to the input.
    if( input.size() == 1 )
    {
      output.assign ( input.begin(), input.end() );
      return;
    }

    // Useful typedefs.
    typedef typename Points::const_iterator ConstIterator;
    typedef typename Points::value_type PointType;
    typedef ParametricPoint < PointType > UPoint;
    typedef std::vector < UPoint > ParametricPoints;

    ParametricPoints points;
    points.reserve( input.size() );

    unsigned int count ( 0 );
    for( ConstIterator iter = input.begin(); iter != input.end(); ++iter )
    {
      UPoint p;
      p.p = *iter;
      p.u = count++;

      points.push_back ( p );
    }

    const unsigned int size ( points.size() );

    // Run through the line segments.
    for( unsigned int i = 0; i < size - 1; ++i )
    {
      UPoint point0 ( points.at( i ) );
      UPoint point1 ( points.at( i + 1 ) );

      // bisect
      bisect ( point0, point1, points );
    }

    std::sort ( points.begin(), points.end() );

    // Clear the output.
    output.clear();

    // Reserve enough room.
    output.reserve( points.size() );

    // Copy the answer.
    for( typename ParametricPoints::const_iterator iter = points.begin(); iter != points.end(); ++iter )
      output.push_back ( iter->p );
  }
}


#endif //__MAGRATHEA_RESAMPLE_H__
