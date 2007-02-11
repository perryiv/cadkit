
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
      mapProj->eastingNorthingToWorld( ossimDpt ( iter->x(), iter->y() ), gpt );
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
    gpt.hgt = OSSIM_DBL_NAN;
    
    ecef = gpt;

    double deltaH = ossimElevManager::instance()->getHeightAboveMSL(gpt);
    if(deltaH == OSSIM_DBL_NAN)
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
  void convertVerticesToEarthCoordinates ( const Points& points, Vertices& vertices )
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
      ossimEcefPoint ecef ( *iter );
      //double normalizationFactor ( ecef.getMagnitude() );

      // Transform to ossimPlanet coordinates
      osg::Vec3 vec;
      vec.x() = ecef.x() * dem;
      vec.y() = ecef.y() * dem;
      vec.z() = ecef.z() * dem;

      vertices.push_back ( vec );
    }
  }
}

#endif
