
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Data/Camera.h"
#include "Minerva/Core/TileEngine/LandModel.h"

#include "GN/Algorithms/Fill.h"
#include "GN/Algorithms/KnotVector.h"
#include "GN/Config/UsulConfig.h"
#include "GN/Evaluate/Point.h"
#include "GN/Interpolate/Global.h"
#include "GN/Splines/Curve.h"

#include "Usul/Math/Constants.h"

using namespace Minerva::Core::Data;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Camera::Camera() : BaseClass(),
  _longitude ( 0.0 ),
  _latitude ( 0.0 ),
  _altitude ( 0.0 ),
  _heading ( 0.0 ),
  _tilt ( 0.0 ),
  _roll ( 0.0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Camera::~Camera()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the longitude.
//
///////////////////////////////////////////////////////////////////////////////

void Camera::longitude ( double lon )
{
  Guard guard ( this );
  _longitude = lon;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the longitude.
//
///////////////////////////////////////////////////////////////////////////////

double Camera::longitude() const
{
  Guard guard ( this );
  return _longitude;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the latitude.
//
///////////////////////////////////////////////////////////////////////////////

void Camera::latitude ( double lat )
{
  Guard guard ( this );
  _latitude = lat;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the latitude.
//
///////////////////////////////////////////////////////////////////////////////

double Camera::latitude() const
{
  Guard guard ( this );
  return _latitude;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the altitude.
//
///////////////////////////////////////////////////////////////////////////////

void Camera::altitude ( double altitude )
{
  Guard guard ( this );
  _altitude = altitude;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the altitude.
//
///////////////////////////////////////////////////////////////////////////////

double Camera::altitude() const
{
  Guard guard ( this );
  return _altitude;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the heading.
//
///////////////////////////////////////////////////////////////////////////////

void Camera::heading ( double heading )
{
  Guard guard ( this );
  _heading = heading;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the heading.
//
///////////////////////////////////////////////////////////////////////////////

double Camera::heading() const
{
  Guard guard ( this );
  return _heading;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the tilt.
//
///////////////////////////////////////////////////////////////////////////////

void Camera::tilt ( double tilt )
{
  Guard guard ( this );
  _tilt = tilt;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tilt.
//
///////////////////////////////////////////////////////////////////////////////

double Camera::tilt() const
{
  Guard guard ( this );
  return _tilt;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the roll.
//
///////////////////////////////////////////////////////////////////////////////

void Camera::roll ( double roll )
{
  Guard guard ( this );
  _roll = roll;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the roll.
//
///////////////////////////////////////////////////////////////////////////////

double Camera::roll() const
{
  Guard guard ( this );
  return _roll;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

Camera::Matrix Camera::viewMatrix ( Minerva::Core::TileEngine::LandModel* landModel ) const
{
  if ( 0x0 == landModel )
    return Matrix();

  Matrix rotation ( landModel->planetRotationMatrix ( this->latitude(), this->longitude(), this->altitude(), this->heading() ) );

  Usul::Math::Vec3d translation;
  rotation.getTranslation ( translation );
  rotation.setTranslation ( Usul::Math::Vec3d ( 0.0, 0.0, 0.0 ) );

  Matrix pitchMatrix, rollMatrix;
  pitchMatrix.makeRotation ( this->tilt(), Usul::Math::Vec3d ( 1.0, 0.0, 0.0 ) );
  rollMatrix.makeRotation ( this->roll(), Usul::Math::Vec3d ( 0.0, 0.0, 1.0 ) );
  
  Matrix answer ( rotation * pitchMatrix * rollMatrix );
  answer.setTranslation ( translation );

  return answer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create view matrix from camera.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  Usul::Math::Matrix44d createViewMatrix ( Camera& camera, Minerva::Core::TileEngine::LandModel* landModel )
  {
    Usul::Math::Matrix44d matrix ( camera.viewMatrix ( landModel ) );
    Usul::Math::Matrix44d viewMatrix;

    matrix.inverse ( viewMatrix );
    return viewMatrix;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the distance in meters between two points.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  double distance ( const Usul::Math::Vec3d& from, const Usul::Math::Vec3d& to, Minerva::Core::TileEngine::LandModel& landModel )
  {
    Usul::Math::Vec3d p0, p1;
    landModel.latLonHeightToXYZ ( from[0], from[1], from[2], p0[0], p0[1], p0[2] );
    landModel.latLonHeightToXYZ ( to[0], to[1], to[2], p1[0], p1[1], p1[2] );
    const double d ( p0.distance ( p1 ) );
    return d;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Use trig to slow down the beginning and end of the path.
//  See http://mathworld.wolfram.com/Cosine.html
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  inline double slowBothEnds ( double param, double mn, double mx )
  {
    param *= Usul::Math::PIE;
    param += Usul::Math::PIE;
    param  = Usul::Math::cos ( param );
    param += 1.0;
    param /= 2.0;

    // Keep in range.
    param = ( ( param < mn ) ? mn : param );
    param = ( ( param > mx ) ? mx : param );

    return param;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Generate the path to animate from start to end.
//
///////////////////////////////////////////////////////////////////////////////

void Camera::generateAnimatePath ( 
    Camera::RefPtr start, 
    Camera::RefPtr end, 
    double percentMidpointHeightAtTransition,
    unsigned int numPoints,
    Minerva::Core::TileEngine::LandModel* landModel, 
    Matrices& matrices )
{
  if ( 0x0 == landModel || false == start.valid() || false == end.valid() || 0 == numPoints )
    return;

  matrices.reserve ( numPoints + 1 );

#if 0
  const double deltaLongitude ( end->longitude() - start->longitude() );
  const double deltaLatitude ( end->latitude() - start->latitude() );
  const double deltaAltitude ( end->altitude() - start->altitude() );

  for ( unsigned int i = 0; i < numPoints; ++i )
  {
    const double u ( static_cast<double> ( i ) / ( numPoints - 1 ) );

    Minerva::Core::Data::Camera::RefPtr currentFrame ( new Minerva::Core::Data::Camera );
    currentFrame->longitude ( start->longitude() + ( u * ( deltaLongitude ) ) );
    currentFrame->latitude ( start->latitude() + ( u * ( deltaLatitude ) ) );
    currentFrame->altitude ( start->altitude() + ( u * ( deltaAltitude ) ) );
    currentFrame->heading ( start->heading() + ( u * ( end->heading() - start->heading() ) ) );
    currentFrame->tilt ( start->tilt() + ( u * ( end->tilt() - start->tilt() ) ) );
    currentFrame->roll ( start->roll() + ( u * ( end->roll() - start->roll() ) ) );

    matrices.push_back ( Detail::createViewMatrix ( *currentFrame, landModel ) );
  }

  // Add one more so we end up where we are suppose to.
  matrices.push_back ( Detail::createViewMatrix ( *end, landModel ) );
#else

  // Needed below.
  typedef Usul::Errors::ThrowingPolicy < std::runtime_error > ErrorChecker;
  typedef GN::Config::UsulConfig < double, double, unsigned int, ErrorChecker > Config;
  typedef GN::Splines::Curve < Config > Curve;
  typedef Usul::Interfaces::IAnimateMatrices::Matrices Matrices;

  Usul::Math::Vec3d llh1 ( start->longitude(), start->latitude(), start->altitude() );
  Usul::Math::Vec3d llh2 ( end->longitude(), end->latitude(), end->altitude() );

  // Calculate the midpoint.
  const double distance ( Detail::distance ( llh1, llh2, *landModel ) );
  Usul::Math::Vec3d mid ( ( llh1[0] + llh2[0] ) / 2.0, ( llh1[1] + llh2[1] ) / 2.0, distance / 2.0 );

  // Midpoint has to be as high or higher than first and last points.
  mid[2] = ( ( mid[2] < llh1[2] ) ? llh1[2] : mid[2] );
  mid[2] = ( ( mid[2] < llh2[2] ) ? llh2[2] : mid[2] );

  // Calculate the transition points.
  Usul::Math::Vec3d tp1 ( llh1[0], llh1[1], mid[2] * percentMidpointHeightAtTransition );
  Usul::Math::Vec3d tp2 ( llh2[0], llh2[1], mid[2] * percentMidpointHeightAtTransition );

  // Transition points have to be as high or higher than first and last points.
  tp1[2] = ( ( tp1[2] < llh1[2] ) ? llh1[2] : tp1[2] );
  tp1[2] = ( ( tp1[2] < llh2[2] ) ? llh2[2] : tp1[2] );
  tp2[2] = ( ( tp2[2] < llh1[2] ) ? llh1[2] : tp2[2] );
  tp2[2] = ( ( tp2[2] < llh2[2] ) ? llh2[2] : tp2[2] );

  // Make the control points.
  const Usul::Math::Vec3d llha ( llh1 );
  const Usul::Math::Vec3d llhb ( llh1[0], llh1[1],  mid[2] );
  const Usul::Math::Vec3d llhc ( mid );
  const Usul::Math::Vec3d llhd ( llh2[0], llh2[1],  mid[2] );
  const Usul::Math::Vec3d llhe ( llh2 );

  // Make the curve.
  Curve curve;
  curve.resize ( 3, 4, 5, false );

  curve.knot ( 0 ) = 0.0;
  curve.knot ( 1 ) = 0.0;
  curve.knot ( 2 ) = 0.0;
  curve.knot ( 3 ) = 0.0;
  curve.knot ( 4 ) = 0.5;
  curve.knot ( 5 ) = 1.0;
  curve.knot ( 6 ) = 1.0;
  curve.knot ( 7 ) = 1.0;
  curve.knot ( 8 ) = 1.0;

  curve.controlPoint ( 0, 0 ) = llha[0];
  curve.controlPoint ( 1, 0 ) = llha[1];
  curve.controlPoint ( 2, 0 ) = llha[2];

  curve.controlPoint ( 0, 1 ) = llhb[0];
  curve.controlPoint ( 1, 1 ) = llhb[1];
  curve.controlPoint ( 2, 1 ) = llhb[2];

  curve.controlPoint ( 0, 2 ) = llhc[0];
  curve.controlPoint ( 1, 2 ) = llhc[1];
  curve.controlPoint ( 2, 2 ) = llhc[2];

  curve.controlPoint ( 0, 3 ) = llhd[0];
  curve.controlPoint ( 1, 3 ) = llhd[1];
  curve.controlPoint ( 2, 3 ) = llhd[2];

  curve.controlPoint ( 0, 4 ) = llhe[0];
  curve.controlPoint ( 1, 4 ) = llhe[1];
  curve.controlPoint ( 2, 4 ) = llhe[2];

  // Loop through the path.
  Curve::Vector llh ( curve.dimension(), 0 );
  unsigned int total ( numPoints );
  for ( unsigned int i = 0; i < total; ++i )
  {
    double param ( static_cast < double > ( i ) / static_cast < double > ( total - 1 ) );
    param = Detail::slowBothEnds ( param, 0.0, 1.0 );
    param = Detail::slowBothEnds ( param, 0.0, 1.0 );

    GN::Evaluate::point ( curve, param, llh );

    Minerva::Core::Data::Camera::RefPtr currentFrame ( new Minerva::Core::Data::Camera );
    currentFrame->longitude ( llh[0] );
    currentFrame->latitude ( llh[1] );
    currentFrame->altitude ( llh[2] );
    currentFrame->heading ( start->heading() + ( param * ( end->heading() - start->heading() ) ) );
    currentFrame->tilt ( start->tilt() + ( param* ( end->tilt() - start->tilt() ) ) );
    currentFrame->roll ( start->roll() + ( param * ( end->roll() - start->roll() ) ) );

    matrices.push_back ( Detail::createViewMatrix ( *currentFrame, landModel ) );
  }

#endif
}
