
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
#include "Usul/Math/Functions.h"
#include "Usul/Math/Angle.h"

#include "osgUtil/IntersectVisitor"

#include "ossimPlanet/ossimPlanetLand.h"

#ifdef _MSC_VER 
# ifdef _DEBUG
#  include <windows.h>
# endif
#endif

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
_zoomMax ( 1 ),
_rotateSpeed ( 8.0 ),
_elevationSpeed ( 0.75 ),
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
  _landModel = 0x0;
  _primary = 0x0;
  _planet = 0x0;
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
  _hpr.x() = 0;
  _hpr.y() = 0;
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

  const double speed ( this->rotateSpeed() * - ( _zoom + 1 ) );

  _lon = Detail::wrap ( _lon + ( x * speed ), _primary->minLon(), _primary->maxLon() );
  _lat += ( y * speed );
  if( _lat > 90.0 )
  {
    _lat = Usul::Math::clamp ( 90.0 - ( _lat - 90.0), -90.0, 90.0 );
    _lon = Detail::wrap( _lon + 180.0, _primary->minLon(), _primary->maxLon());

    _hpr.x() = Detail::wrap( _hpr.x() + 180.0, -180.0, 180.0 );

 
  }
//  osg::Matrixd orient ( this->viewMatrix() );
//  osg::Quat quat ( orient.getRotate() );
//
//  //quat.set( 0.0f, 0.0f, quat.z(), quat.w() );
//  double degrot;
//  osg::Vec3f vec;
//  quat.getRotate( degrot, vec );
//  _compass->setHeadingRelativeToNorth( osg::Vec2( 0.0f , 90.0f ) , osg::Vec2( _lon , _lat ) );
//  
//  
//
//  osg::EllipsoidModel model (1.0, _primary->polarRadius() / _primary->equatorialRadius() );
//   
//  double nx, ny, nz, px, py, pz;
//
//  model.convertLatLongHeightToXYZ( osg::DegreesToRadians ( 90.0f ),
//                                   osg::DegreesToRadians ( 0.0f),
//                                   0,
//                                   nx,ny,nz);
//
//  model.convertLatLongHeightToXYZ( osg::DegreesToRadians ( _lat ),
//                                   osg::DegreesToRadians ( _lon),
//                                   0,
//                                   px,py,pz);
//  
//  osg::Vec3 north    ( 0.0, 0.0, 1.0 );
//  osg::Vec3 position ( px, py, pz );
//
//  float PdotN = position * north;
//  float NdotN = north * north;
//
//  osg::Vec3 directionToNorth ( north -  ( position * PdotN ) );
//  directionToNorth.y() = 0;
//  directionToNorth.normalize();
//
//  //osg::Vec3 w1 ( ( directionToNorth * PdotN ) / NdotN );
//  //osg::Vec3 w2 ( directionToNorth - w1 );
//  
//  //float rotation (atan2( w1, w2 ) );
//  osg::Vec3 directionHeading( ( sin( _hpr.x() ) * cos( _hpr.y() ) ),
//                              ( sin( _hpr.x() ) * sin( _hpr.y() ) ),
//                              ( cos( _hpr.x() ) ) );
//  directionHeading.y() = 0;
//  directionHeading.normalize();
//
//  //double rotation ( osg::RadiansToDegrees ( Usul::Math::Angle < double, 3 >::get ( directionHeading , directionToNorth ) ) );
//  float rotation ( osg::RadiansToDegrees( acos( directionHeading * directionToNorth ) ) );
//  //float correction = osg::RadiansToDegrees( atan ( ( vec.x() * float( degrot ) ) / ( vec.z() * float( degrot ) ) ) );
//  //float rotation = osg::RadiansToDegrees( atan ( (  ( 1 - pz ) / ( - px ) ) ) ); 
//  //_compass->setHeading( _compass->getHeading() - osg::RadiansToDegrees( float( rotation ) ) );
//
//  /*if( _lon < -90 )
//    _compass->setHeading( -rotation  );
//  else*/
//    _compass->setHeading( rotation  );
//
//#ifdef _MSC_VER
//# ifdef _DEBUG
//  std::ostringstream os;
//  os << "To North: " << directionToNorth.x() << " " << directionToNorth.y() << " " << directionToNorth.z() << " "
////    << "Heading: " << directionHeading.x() << " " << directionHeading.y() << " " << directionHeading.z() << " "
//    << "Lat/Lon: "  << _lat << " " << _lon << " "
//     << "Heading/Pitch: " << _hpr.x() << " " << _hpr.y() << " "
//     << "Rotation: " << rotation << std::endl;
//  ::OutputDebugStringA ( os.str().c_str() ); 
//# endif
//#endif
//
//  //_compass->setHeading( _compass->getHeading() + ( dx * 180.0f / osg::PI ) );
//  //_compass->setPitch( _hpr.y() );
//
//  _compass->updateCompass();
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
  double dy ( point1.y() - point0.y() );

  double dZoom ( this->_calculateLinearSpeed ( this->elevationSpeed(), dy ) );

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

  //rotate compass
  // _compass->setHeading( _compass->getHeading() + _hpr.x() );
  //_compass->setPitch( _hpr.y() );

  //_compass->updateCompass();
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
    _landModel = planet->land()->model().get();
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
      _planet->land()->model()->inverse( losPt, point );
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


//////////////////////////////////////////////////////////////////////////////
//
//  Calculate the speed given current parameters.
//
///////////////////////////////////////////////////////////////////////////////

double Navigator::_calculateLinearSpeed ( double factor, double displacement ) const
{
  double speed ( ( _zoom + 1 ) * factor * displacement );
  if ( 0.0 == speed )
    return 0.1;
  return speed;
}
