
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSSIM_PROJECT_H__
#define __OSSIM_PROJECT_H__

#include "ossim/base/ossimCommon.h"
#include "ossim/base/ossimConstants.h"
#include "ossim/base/ossimGpt.h"
#include "ossim/base/ossimEcefPoint.h"
#include "ossim/base/ossimGeoidManager.h"
#include "ossim/projection/ossimMapProjection.h"


namespace Magrathea
{
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Convert points to lat/lon.
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < typename Input, typename Output >
  void convertToLatLong( const Input& input, Output& output, ossimMapProjection* mapProj )
  {
    output.reserve ( input.size() );

    for( typename Input::const_iterator iter = input.begin(); iter != input.end(); ++iter )
    {
      ossimGpt gpt;
      mapProj->eastingNorthingToWorld( ossimDpt ( (*iter)[0], (*iter)[1] ), gpt );

      double deltaH = ossimElevManager::instance()->getHeightAboveMSL(gpt);
#if OSSIM_MINOR_VERSION_NUMBER < 7
      if(deltaH == OSSIM_DBL_NAN)
#else
      if( ossim::isnan (deltaH))
#endif
      {
         deltaH = 0.0;
      }

      gpt.height( deltaH + ossimGeoidManager::instance()->offsetFromEllipsoid(gpt) );
      output.push_back ( gpt );
    }
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Project to ellipsiod.  Assumes vec is already in lat/lon
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < class Vector >
  void projectToEllipsoid ( Vector& vec, float zOffset )
  {
    ossimEcefPoint ecef;
    ossimGpt dummy;
    ecef = dummy;
    double normalizationFactor = ecef.getMagnitude();

    ossimGpt gpt ( vec.y(), vec.x() );

    ecef = gpt;

    gpt.height( ossimGeoidManager::instance()->offsetFromEllipsoid( gpt ) );
    
    gpt.hgt += zOffset;

    // Transform to ossimPlanet coordinates
    ecef = gpt;
    vec.x() = ecef.x()/normalizationFactor;
    vec.y() = ecef.y()/normalizationFactor;
    vec.z() = ecef.z()/normalizationFactor;
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Convert to earth coordinates.
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < class Vector >
  void convertToEarthCoordinates ( Vector& vec, ossimMapProjection* mapProj, float zOffset )
  {
    ossimGpt gpt;
    ossimEcefPoint ecef;
    ecef = gpt;
    double normalizationFactor = ecef.getMagnitude();

    mapProj->eastingNorthingToWorld( ossimDpt ( vec.x(), vec.y() ), gpt );

    //std::cerr << "Lat: " << gpt.lat << " Long: " << gpt.lon << std::endl;

    double deltaH = ossimElevManager::instance()->getHeightAboveMSL(gpt);
#if OSSIM_MINOR_VERSION_NUMBER < 7
      if(deltaH == OSSIM_DBL_NAN)
#else
      if( ossim::isnan (deltaH))
#endif
    {
      deltaH = 0.0;
    }

    gpt.height( deltaH + ossimGeoidManager::instance()->offsetFromEllipsoid(gpt) );

    gpt.hgt += zOffset;

    // Transform to ossimPlanet coordinates
    ecef = gpt;
    vec.x() = ecef.x()/normalizationFactor;
    vec.y() = ecef.y()/normalizationFactor;
    vec.z() = ecef.z()/normalizationFactor;
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Convert to earth coordinates.  Assumes vec is already in lat/lon
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < class Vector >
  void convertToEarthCoordinates ( Vector& vec, float zOffset )
  {
    ossimEcefPoint ecef;
    ossimGpt dummy;
    ecef = dummy;
    double normalizationFactor = ecef.getMagnitude();

    ossimGpt gpt ( vec.y(), vec.x() );
    
    ecef = gpt;

    double deltaH = ossimElevManager::instance()->getHeightAboveMSL(gpt);
#if OSSIM_MINOR_VERSION_NUMBER < 7
      if(deltaH == OSSIM_DBL_NAN)
#else
      if( ossim::isnan (deltaH))
#endif
    {
       deltaH = 0.0;
    }

    gpt.height( deltaH + ossimGeoidManager::instance()->offsetFromEllipsoid(gpt) );
    
    gpt.hgt += zOffset;

    // Transform to ossimPlanet coordinates
    ecef = gpt;
    vec.x() = ecef.x()/normalizationFactor;
    vec.y() = ecef.y()/normalizationFactor;
    vec.z() = ecef.z()/normalizationFactor;
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Convert to earth coordinates.  Assumes vertices are already in lat/lon
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < class VecArray >
  void convertVerticesToEarthCoordinates ( VecArray& vertices, float zOffset )
  {
    for( typename VecArray::iterator iter = vertices.begin(); iter != vertices.end(); ++iter )
    {
      convertToEarthCoordinates( (*iter), zOffset );
    }
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Convert to earth coordinates.
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < class VecArray >
  void convertVerticesToEarthCoordinates ( VecArray& vertices, ossimMapProjection* mapProj, float zOffset )
  {
    for( typename VecArray::iterator iter = vertices.begin(); iter != vertices.end(); ++iter )
    {
      convertToEarthCoordinates( (*iter), mapProj, zOffset );
    }
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Create earth coordinates from lat lon points.
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < class Points, class Vertices >
  void convertVerticesToEarthCoordinates ( const Points& points, Vertices& vertices, float zOffset = 0.0 )
  {
    vertices.clear();
    vertices.reserve ( points.size() );

    ossimEcefPoint e;
    ossimGpt dummy;
    e = dummy;
    double normalizationFactor ( e.getMagnitude() );
    double dem ( 1 / normalizationFactor );

    for( typename Points::const_iterator iter = points.begin(); iter != points.end(); ++ iter )
    {
      typename Points::value_type p ( *iter );
      ossimGpt point ( p[ 1 ], p[0], p[2] );
      point.hgt += zOffset;
      ossimEcefPoint ecef ( point );
      //double normalizationFactor ( ecef.getMagnitude() );

      // Transform to ossimPlanet coordinates
      typename Vertices::value_type vec;
      vec[0] = ecef.x() * dem;
      vec[1] = ecef.y() * dem;
      vec[2] = ecef.z() * dem;

      vertices.push_back ( vec );
    }
  }
}

#endif
