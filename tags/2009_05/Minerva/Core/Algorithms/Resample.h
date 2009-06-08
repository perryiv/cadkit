
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_ALGORITHMS_RESAMPLE_H__
#define __MINERVA_CORE_ALGORITHMS_RESAMPLE_H__

#include "Usul/Math/Absolute.h"
#include "Usul/Interfaces/IElevationDatabase.h"

#include <algorithm>

namespace Minerva {
namespace Core {
namespace Algorithms {
namespace Detail
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
  void bisect ( Point &point0, Point &point2, ParmetricPoints& answer, unsigned int depth, unsigned int maximumDepth, Usul::Interfaces::IUnknown *caller )
  {
    if( depth > maximumDepth )
      return;

    double u1 ( ( point0.u + point2.u ) * 0.50 );

    double lat1 ( ( point0.p[1] + point2.p[1] ) * 0.50 );
    double lon1 ( ( point0.p[0] + point2.p[0] ) * 0.50 );
    double height1 ( ( point0.p[2] + point2.p[2] ) * 0.50 );

    // Calculate the height of the mid-point.
    Usul::Interfaces::IElevationDatabase::QueryPtr elevation ( caller );
    const double height ( ( elevation.valid() ) ? elevation->elevationAtLatLong ( lat1, lon1 ) : 0.0 );

    /// one cm resolution
    const double errorFactor ( 0.01 );

    if( Usul::Math::absolute ( height - height1 ) < errorFactor )
      return;

    Point midPoint;
    midPoint.p = typename Point::PointType ( lon1, lat1, height );
    midPoint.u = u1;

    answer.push_back ( midPoint );

    ++depth;
    bisect ( point0, midPoint, answer, depth + 1, maximumDepth, caller );
    bisect ( midPoint, point2, answer, depth + 1, maximumDepth, caller );
  }
}

  template < typename Points >
  void resample ( const Points& input, Points& output, unsigned int maximumDepth, Usul::Interfaces::IUnknown *caller )
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
    typedef Detail::ParametricPoint < PointType > UPoint;
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

      unsigned int depth ( 0 );

      // bisect.
      bisect ( point0, point1, points, depth, maximumDepth, caller );
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
}
}

#endif //__MINERVA_CORE_ALGORITHMS_RESAMPLE_H__
