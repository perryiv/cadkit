
//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAGRATHEA_NAVIGATOR_H__
#define __MAGRATHEA_NAVIGATOR_H__

#include "Magrathea/Export.h"

#include "osg/Referenced"
#include "osg/Vec3d"
#include "osg/Matrixd"

#include "ossimPlanet/ossimPlanetLandModel.h"
#include "ossimPlanet/ossimPlanetPrimaryBody.h"
#include "ossimPlanet/ossimPlanet.h"

namespace Magrathea {

class MAGRATHEA_EXPORT Navigator : public osg::Referenced
{
public:
  typedef osg::Referenced BaseClass;

  Navigator( ossimPlanetPrimaryBody* );

  void                  setLatLonHeight(double lat, double lon, double height);

  osg::Matrixd          orientationLsrMatrix() const;

  void                  viewMatrix ( const osg::Matrixd& m );
  osg::Matrixd          viewMatrix() const;

  void                  home();
  void                  rotate    ( const osg::Vec3f& point0, const osg::Vec3f& point1 );
  void                  rotateLOS ( const osg::Vec3f& point0, const osg::Vec3f& point1 );
  void                  elevation ( const osg::Vec3f& point0, const osg::Vec3f& point1 );
  void                  zoomLOS   ( const osg::Vec3f& point0, const osg::Vec3f& point1 );
  void                  look      ( const osg::Vec3f& point0, const osg::Vec3f& point1 );

  // Set the planet.
  void                  planet( ossimPlanet * planet );
 
  /// Get/Set rotate speed.
  void                  rotateSpeed ( double speed ) { _rotateSpeed = speed; }
  double                rotateSpeed () const         { return _rotateSpeed; }

  /// Get/Set elevation speed.
  void                  elevationSpeed ( double speed ) { _elevationSpeed = speed; }
  double                elevationSpeed () const         { return _elevationSpeed; }
  
  /// Get/Set the pitch speed.
  void                  pitchSpeed ( double speed ) { _pitchSpeed = speed; }
  double                pitchSpeed () const         { return _pitchSpeed; }

  /// Get/Set the yaw speed.
  void                  yawSpeed ( double speed ) { _yawSpeed = speed; }
  double                yawSpeed () const         { return _yawSpeed; }

protected:
  virtual ~Navigator();

  // Get the elevation given the current zoom.
  double                  _elevation() const;

  /// Calculate where the line of sight intersects the planet.
  bool                    _losIntersectionPoint ( osg::Vec3d& point );

  /// Calculate the speed given current parameters.
  double                  _calculateLinearSpeed ( double factor, double displacement ) const;

private:
  // Eye position in lat, lon.
  double _lat;
  double _lon;
  double _zoom;
  osg::Vec3d _hpr; // heading ( yaw ), pitch and roll.  Should probably be a quaternion.

  double _zoomMin;
  double _zoomMax;

  double _rotateSpeed;
  double _elevationSpeed;
  double _pitchSpeed;
  double _yawSpeed;

  osg::ref_ptr<ossimPlanetLandModel>   _landModel;
  osg::ref_ptr<ossimPlanetPrimaryBody> _primary;

  // The planet
  osg::ref_ptr < ossimPlanet > _planet;
};

}

#endif // __MAGRATHEA_NAVIGATOR_H__
