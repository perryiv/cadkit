
//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Magrathea/Navigator.h"

#include "Usul/Math/MinMax.h"
#include "Usul/Math/UMath.h"

#include "osgUtil/IntersectVisitor"

#include "ossimPlanet/ossimPlanetLand.h"

using namespace Magrathea;


//////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Navigator::Navigator( ossimPlanetPrimaryBody * primary ) : BaseClass(),
_lat( -0.0 ),
_lon (0.0 ),
_zoom ( -4.2 ),
_hpr( 0.0, 0.0, 0.0 ),
_zoomMin ( -4.2 ),
_zoomMax ( 0.001 ),
_rotateSpeed ( 2.5 ),
_elevationSpeed ( 0.25 ),
_pitchSpeed ( 0.5 ),
_yawSpeed ( 20 ),
_landModel ( 0x0 ),
_primary ( primary ),
_planet ( 0x0 )
{
}


//////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Navigator::~Navigator()
{
}


//////////////////////////////////////////////////////////////////////////////
//
//  Set the lat, lon, and height.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::setLatLonHeight(double lat, double lon, double height)
{
  _lat = lat;
  _lon = lon;

  _zoom = Usul::Math::clamp ( -( 1.0 + ( height / _primary->radius ( _lat ) ) ), _zoomMin, _zoomMax );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation given the current zoom.
//
///////////////////////////////////////////////////////////////////////////////

double Navigator::_elevation() const
{
  return ( -_zoom - 1.0 ) * _primary->radius ( _lat );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Create the orientation matrix.
//
///////////////////////////////////////////////////////////////////////////////

osg::Matrixd Navigator::orientationLsrMatrix() const
{
  osg::Matrixd output;

  _landModel->lsrMatrix(osg::Vec3d( _lat, _lon, this->_elevation() * _landModel->getNormalizationScale() ),
                         output,
                         _hpr[0] );
  output(3,0) = 0.0;
  output(3,1) = 0.0;
  output(3,2) = 0.0;

  osg::Matrix m1, m2;
  m1.makeRotate( _hpr[1], osg::Vec3 ( 1.0, 0.0, 0.0 ) );
  m2.makeRotate( _hpr[2], osg::Vec3 ( 0.0, 1.0, 0.0 ) );
  osg::Matrix orien ( m1 * m2 );

  return orien * output;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Set internal values by using the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::viewMatrix ( const osg::Matrixd& m )
{
}


//////////////////////////////////////////////////////////////////////////////
//
//  Return the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

osg::Matrixd Navigator::viewMatrix() const
{
  osg::EllipsoidModel model (1.0, _primary->polarRadius() / _primary->equatorialRadius() );
   
  double x, y, z;

  model.convertLatLongHeightToXYZ( osg::DegreesToRadians ( _lat ),
                                   osg::DegreesToRadians ( _lon),
                                   this->_elevation()/ _primary->equatorialRadius(),
                                   x,y,z);

  return orientationLsrMatrix()*osg::Matrixd::translate(x,y,z);
}


//////////////////////////////////////////////////////////////////////////////
//
//  Go to the home position.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::home()
{
  _zoom = _zoomMin;
  _hpr.set(0.0, 0.0, 0.0);
  _lat = 0.0;
  _lon = -90.0;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Wrap the value if it's outside the range.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < typename T > T wrap ( T x, T a, T b )
  {
    if( a == b )
      return a;

    T z = x < a ? b : a;
    return ::fmod ( x - z, b - a ) + z;
  }
}

//////////////////////////////////////////////////////////////////////////////
//
//  Rotate.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::rotate ( const osg::Vec3f& point0, const osg::Vec3f& point1 )
{
  float heading ( osg::DegreesToRadians ( _hpr.x() ) );

  float s ( Usul::Math::sin( heading ) );
  float c ( Usul::Math::cos( heading ) );

  double dx ( point1.x() - point0.x() );
  double dy ( point1.y() - point0.y() );

  double x ( dx * c + dy * s );
  double y ( -dx * s + dy * c );

  const double speed ( this->rotateSpeed() * -_zoom );

  _lon = Detail::wrap ( _lon + ( x * speed ), _primary->minLon(), _primary->maxLon() );
  _lat += ( y * speed );
  if( _lat > 90.0 )
  {
    _lat = Usul::Math::clamp ( 90.0 - ( _lat - 90.0), -90.0, 90.0 );
    _lon = Detail::wrap( _lon + 180.0, _primary->minLon(), _primary->maxLon());

    _hpr.x() = Detail::wrap( _hpr.x() + 180.0, -180.0, 180.0 );
  }
}


//////////////////////////////////////////////////////////////////////////////
//
//  Rotate around the line of sight.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::rotateLOS ( const osg::Vec3f& point0, const osg::Vec3f& point1 )
{
  double dx ( point1.x() - point0.x() );
  double dy ( point1.y() - point0.y() );

  osg::Vec3d losXYZ;
  if ( this->_losIntersectionPoint ( losXYZ ) )
  {
    osg::Matrixd eyeLsrMatrix ( this->viewMatrix() );
    osg::Vec3d eye(eyeLsrMatrix(3,0), eyeLsrMatrix(3,1), eyeLsrMatrix(3,2));
    double distance = ( losXYZ-eye ).length();
    eyeLsrMatrix(3,0) = 0.0;
    eyeLsrMatrix(3,1) = 0.0;
    eyeLsrMatrix(3,2) = 0.0;
    
    double rotationZAmount = dx*(10* osg::PI / 180.0 );
    double rotationPitchAmount = dy*(10* osg::PI / 180.0 );
    osg::Vec3d xn = osg::Vec3d(1.0, 0.0, 0.0);
    osg::Vec3d zn = losXYZ;
    zn.normalize();
    osg::Matrixd r = osg::Matrix::rotate(rotationZAmount, zn);
    eyeLsrMatrix = eyeLsrMatrix*r;

    eyeLsrMatrix = eyeLsrMatrix*osg::Matrix::rotate(rotationPitchAmount, osg::Vec3d(eyeLsrMatrix(0,0),
                                                                                    eyeLsrMatrix(0,1),
                                                                                    eyeLsrMatrix(0,2)));
    osg::Vec3d newPoint = losXYZ + osg::Vec3d(eyeLsrMatrix(2, 0),
                                                 eyeLsrMatrix(2, 1),
                                                 eyeLsrMatrix(2, 2))*distance;
    osg::Vec3d eyeLlh;
    _landModel->inverse(newPoint, eyeLlh);

    this->setLatLonHeight(eyeLlh[0],
                          eyeLlh[1],
                          eyeLlh[2]);

    /*osg::Matrixd tempView ( viewMatrix() );
    osg::Matrixd localLsr;

    _landModel->lsrMatrix(eyeLlh, localLsr);
    osg::Vec3d tempHpr;
    
    mkUtils::matrixToHpr(tempHpr, localLsr, eyeLsrMatrix);
    
    _hpr = tempHpr;
    _hpr.z() = 0.0;
    _hpr.x() = Usul::Math::clamp ( _hpr.x(), -180.0, 180.0);
    _hpr.y() = Usul::Math::clamp ( _hpr.y(), 0.0, 180.0);*/
  }
}


//////////////////////////////////////////////////////////////////////////////
//
//  Change elevation.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::elevation ( const osg::Vec3f& point0, const osg::Vec3f& point1 )
{
  double dx ( point1.x() - point0.x() );
  double dy ( point1.y() - point0.y() );

  const double dZoom ( _zoom * this->elevationSpeed() * dy );

  double zoom ( Usul::Math::clamp ( _zoom + dZoom, _zoomMin, _zoomMax ) );
  _zoom = zoom;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Zoom along the line of sight.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::zoomLOS   ( const osg::Vec3f& point0, const osg::Vec3f& point1 )
{
  double dx ( point1.x() - point0.x() );
  double dy ( point1.y() - point0.y() );

  osg::Vec3d losXYZ;
  if ( this->_losIntersectionPoint ( losXYZ ) )
  {
    osg::Matrixd eyeLsrMatrix ( viewMatrix() );
    osg::Vec3d eye(eyeLsrMatrix(3,0), eyeLsrMatrix(3,1), eyeLsrMatrix(3,2));
    osg::Vec3d posXYZ;
    osg::Vec3d posLlh;
    osg::Vec3d tempHpr;

    osg::Vec3d deltaV ( eye - losXYZ );

    double distanceTarget = 0.0;
    double distanceCurrent = deltaV.length();
    double minDistance     = 64.0/ _landModel->getNormalizationScale();

    distanceTarget = (distanceTarget + distanceCurrent * .1 * dy);
    double distance = distanceCurrent + distanceTarget;
    if(distance < minDistance)
    {
      distance = minDistance;
    }

    deltaV.normalize();

    posXYZ = losXYZ + deltaV * distance;

    _landModel->inverse(posXYZ, posLlh);

    this->setLatLonHeight(posLlh[0], posLlh[1], posLlh[2] * _landModel->getNormalizationScale());

    /*osg::Matrixd localLsr;
    _landModel->lsrMatrix(posLlh, localLsr);

    mkUtils::matrixToHpr(tempHpr, localLsr, eyeLsrMatrix);
    _hpr = tempHpr;
    _hpr.z() = 0.0;
    _hpr.x() = Usul::Math::clamp ( _hpr.x(), -180.0, 180.0);
    _hpr.y() = Usul::Math::clamp ( _hpr.y(), 0.0, 180.0);*/
  }
}


//////////////////////////////////////////////////////////////////////////////
//
//  Change the look direction.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::look ( const osg::Vec3f& point0, const osg::Vec3f& point1 )
{
  float pitchSpeed ( this->pitchSpeed() );
  float yawSpeed ( this->yawSpeed() );

  double dx ( point1.x() - point0.x() );
  double dy ( point1.y() - point0.y() );

  double heading ( Detail::wrap < float > ( _hpr.x() - dx * yawSpeed, -180.0f, 180.0f ) );
  _hpr.x() = heading;

  double pitch ( Usul::Math::clamp ( _hpr.y() - dy * pitchSpeed, 0.0, osg::DegreesToRadians ( 90.0 ) ) );
  _hpr.y() = pitch;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Set the planet.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::planet( ossimPlanet * planet )
{
  if( 0x0 != planet )
  {
    _planet = planet;
    _landModel = planet->getLand()->getModel().get();
  }
}


//////////////////////////////////////////////////////////////////////////////
//
//  Calculate where the line of sight intersects the planet.
//
///////////////////////////////////////////////////////////////////////////////

bool Navigator::_losIntersectionPoint ( osg::Vec3d& point )
{
  bool hitFound ( false );

  if( 0x0 != _planet )
  {
    if( _planet->getComputeIntersectionFlag() )
    {
      osg::Vec3d losPt ( _planet->getLineOfSitePoint() );
      _planet->getLand()->getModel()->inverse( losPt, point );
      hitFound = true;
    }
    else
    {
      osg::Matrixd m ( this->viewMatrix() );

      osgUtil::IntersectVisitor iv;
      
      osg::Vec3d center;
      osg::Vec3d look(- m(2,0),-m(2,1),-m(2,2));
      osg::Vec3d eye(m(3,0),m(3,1),m(3,2));
      
      center = eye + look*100.0;

      /*osg::Vec3d center, up, eye;
      m.getLookAt ( eye, center, up );*/
      
      osg::ref_ptr<osg::LineSegment> segLookVector ( new osg::LineSegment );
      segLookVector->set( eye, center );
      iv.addLineSegment( segLookVector.get() );
      
      _planet->accept(iv);
      if (iv.hits())
      {
         osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(segLookVector.get());
         if (!hitList.empty())
         {
           point = hitList.front().getWorldIntersectPoint();
            //osg::Vec3d intersection = hitList.front().getWorldIntersectPoint();
            //_planet->getLand()->getModel()->inverse( intersection, point );
            hitFound = true;
         }
      }
    }
  }

  return hitFound;
}
