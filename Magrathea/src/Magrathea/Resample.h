
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

#include <algorithm>

namespace Magrathea
{
  template < typename Points >
  class Resample
  {
  public:
    typedef typename Points::value_type       Point;
    typedef typename Points::const_iterator   ConstIterator;

    Resample() {}

    void operator() ( const Points& in, Points& out );

  private:
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

    typedef ParametricPoint < Point > UPoint;
    typedef std::vector < UPoint > ParametricPoints;

    struct TodoFunctor
    {
      typedef std::list < TodoFunctor > TodoStack;
      
      TodoFunctor ( const UPoint& point0, const UPoint& point2 );

      void operator() ( TodoStack& stack, ParametricPoints& answer );

    private:
      UPoint _point0, _point2;
    };
  };


  template < typename Points >
  Resample< Points >::TodoFunctor::TodoFunctor( const UPoint& point0, const UPoint& point2 ) : 
  _point0 ( point0 ), 
  _point2 ( point2 )
  {
  }

  template < typename Points >
  void Resample< Points >::TodoFunctor::operator () ( TodoStack& stack, ParametricPoints& answer )
  {
    UPoint & point0 ( _point0 );
    UPoint & point2 ( _point2 );

    double u1 ( ( point0.u + point2.u ) * 0.50 );

    double lat1 ( ( point0.p.latd() + point2.p.latd() ) * 0.50 );
    double lon1 ( ( point0.p.lond() + point2.p.lond() ) * 0.50 );
    double height1 ( ( point0.p.height() + point2.p.height() ) * 0.50 );

    // Calculate the height of the mid-point.
    typename UPoint::PointType point1 ( lat1, lon1 );
    double deltaH(  ossimElevManager::instance()->getHeightAboveMSL( point1 ) );
    if(deltaH == OSSIM_DBL_NAN)
    {
      deltaH = 0.0;
    }

    point1.height( deltaH + ossimGeoidManager::instance()->offsetFromEllipsoid( point1 ) );

    /// one cm resolution
    double errorFactor ( 0.01 );

    if( Usul::Math::absolute ( point1.height() - height1 ) < errorFactor )
      return;

    UPoint midPoint;
    midPoint.p = point1;
    midPoint.u = u1;

    answer.push_back ( midPoint );

    stack.push_back ( TodoFunctor ( point0, midPoint ) );
    stack.push_back ( TodoFunctor ( midPoint, point2 ) );
  }


  template < typename Points >
  void Resample< Points >::operator () ( const Points& input, Points& output )
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

      TodoFunctor::TodoStack stack;

      TodoFunctor functor ( point0, point1 );
      stack.push_back ( functor );

      for( TodoFunctor::TodoStack::iterator iter = stack.begin(); iter != stack.end(); ++iter )
      {
        TodoFunctor &f ( *iter );
        f( stack, points );
      }
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
  void bisect ( Point &point0, Point &point2, ParmetricPoints& answer, unsigned int& depth )
  {
    unsigned int maximiumDepth ( ::pow ( 2.0, 8 ) );
    if( depth > maximiumDepth )
      return;

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

    /// one cm resolution
    double errorFactor ( 0.01 );

    if( Usul::Math::absolute ( point1.height() - height1 ) < errorFactor /*&& distance0 < 2.0 && distance2 < 2.0*/ )
      return;

    Point midPoint;
    midPoint.p = point1;
    midPoint.u = u1;

    answer.push_back ( midPoint );

    ++depth;
    bisect ( point0, midPoint, answer, depth );
    bisect ( midPoint, point2, answer, depth );
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

      unsigned int depth ( 0 );

      // bisect.
      bisect ( point0, point1, points, depth );
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
