
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interpolates a path with a B-Spline curve and animates through the curve.
//
///////////////////////////////////////////////////////////////////////////////

#include "PathAnimation/CurvePlayer.h"
#include "PathAnimation/CameraPath.h"

#include "GN/Algorithms/KnotVector.h"
#include "GN/Algorithms/Parameterize.h"
#include "GN/Evaluate/Point.h"
#include "GN/Interpolate/Global.h"

#include "Usul/Interfaces/IRenderLoop.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Math/Transpose.h"
#include "Usul/Trace/Trace.h"

#include "osg/Vec3d"
#include "osg/Matrixd"

#include <algorithm>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CurvePlayer::CurvePlayer() : BaseClass(),
  _playing ( false ),
  _curve(),
  _current ( 0 ),
  _step ( 0.01 ),
  _renderLoop ( false )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CurvePlayer::~CurvePlayer()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the step size.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::stepSize ( Parameter delta )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _step = delta;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the step size.
//
///////////////////////////////////////////////////////////////////////////////

CurvePlayer::Parameter CurvePlayer::stepSize() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _step;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play the animation from the current paramater.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::_play ( const CameraPath *path, unsigned int degree, Usul::Interfaces::IUnknown *caller, bool reverseOrder )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Initialize.
  this->playing ( false );
  _current = 0;

  // Required interface.
  Usul::Interfaces::IRenderLoop::QueryPtr rl ( caller );
  if ( false == rl.valid() )
    return;

  // Try to make the curve.
  this->_interpolate ( path, degree, reverseOrder );

  // Did it work?
  if ( false == _curve.valid() )
    return;

  // Set current parameter.
  _current = _curve.firstKnot();

  // We are now playing.
  this->playing ( true );

  // Save current render-loop state and turn it on.
  _renderLoop = rl->renderLoop();
  rl->renderLoop ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play the animation forward from the current paramater.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::playForward ( const CameraPath *path, unsigned int degree, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  this->_play ( path, degree, caller, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play the animation backward from the current paramater.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::playBackward ( const CameraPath *path, unsigned int degree, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  this->_play ( path, degree, caller, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the animation.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::stopPlaying ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  this->playing ( false );

  Usul::Interfaces::IRenderLoop::QueryPtr rl ( caller );
  if ( rl.valid ( ) )
  {
    Guard guard ( this );
    rl->renderLoop ( _renderLoop );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::playing ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _playing = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we playing?
//
///////////////////////////////////////////////////////////////////////////////

bool CurvePlayer::playing() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _playing;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the curve.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::clear()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _curve.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Interpolate the path.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::_interpolate ( const CameraPath *path, unsigned int degree, bool reverseOrder )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle bad input.
  if ( 0x0 == path )
    return;

  // For readability.
  typedef Curve::IndependentType IndependentType;
  typedef Curve::SizeType SizeType;
  typedef Curve::ErrorCheckerType ErrorCheckerType;
  typedef GN::Algorithms::KnotVector < IndependentSequence, ErrorCheckerType > KnotVectorBuilder;
  typedef GN::Algorithms::Parameterize < IndependentSequence, DependentContainer, Curve::Power, ErrorCheckerType > Parameterize;

  // Clear what we have.
  _curve.clear();

  // Get a copy of the values.
  CameraPath::Values values;
  path->values ( values, reverseOrder );

  // Make sure there are no adjacent values that are identical.
  values.erase ( std::unique ( values.begin(), values.end(), CameraPath::EqualValue() ), values.end() );

  // Now that we have unique frames, make sure there are enough.
  if ( values.size() < Curve::Limits::MIN_NUM_CTR_PTS )
    return;

  // Container for the data.
  DependentContainer points;
  points.reserve ( values.size() );

  // Separate container for just the eye positions.
  DependentContainer eyes;
  eyes.reserve ( values.size() );

  // Loop through the values.
  for ( CameraPath::Values::const_iterator i = values.begin(); i != values.end(); ++i )
  {
    // Get the data.
    const CameraPath::Triplet &value ( *i );
    const CameraPath::Vec3d &eye    ( value[0] );
    const CameraPath::Vec3d &center ( value[1] );
    const CameraPath::Vec3d &up     ( value[2] );

    // Make point for just the eye position.
    DependentSequence point;
    point.push_back ( eye[0] );
    point.push_back ( eye[1] );
    point.push_back ( eye[2] );

    // Append the eye position.
    eyes.push_back ( point );

    // Continue adding the rest of the dimensions.
    point.push_back ( center[0] );
    point.push_back ( center[1] );
    point.push_back ( center[2] );
    point.push_back ( up[0] );
    point.push_back ( up[1] );
    point.push_back ( up[2] );

    // Append all dimensions (including eye position).
    points.push_back ( point );
  }

  // Transpose so that the first index is the dimension.
  Usul::Math::transpose ( eyes );
  Usul::Math::transpose ( points );

  // Fit parameters to the eye positions.
  IndependentSequence params;
  params.reserve ( values.size() );
  Parameterize::fit ( eyes, GN::Algorithms::Constants::CENTRIPETAL_FIT, params );

  // Make the knot vector. Size it for interpolation.
  const SizeType order ( Usul::Math::minimum<SizeType> ( degree + 1, params.size() ) );
  IndependentSequence knots;
  knots.resize ( params.size() + order );
  KnotVectorBuilder::build ( params, order, knots );

  // Interpolate the points.
  GN::Interpolate::global ( order, params, knots, points, _curve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go to the current parameter.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::go ( Parameter u, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Get the needed interface.
  Usul::Interfaces::IViewMatrix::QueryPtr vm ( caller );
  if ( false == vm.valid() )
    return;

  // Make sure we have a valid curve.
  if ( false == _curve.valid() )
    return;

  // Make sure the parameter is in range.
  if ( ( u < _curve.firstKnot() ) || ( u > _curve.lastKnot() ) )
    return;

  // Check number of dependent variables.
  if ( 9 != _curve.numDepVars() )
    return;

  // Evaluate the dependent variables. Have to size the point!
  Curve::Vector point ( _curve.numDepVars() );
  GN::Evaluate::point ( _curve, u, point );

  // Get the point's components.
  osg::Vec3d eye    ( point[0], point[1], point[2] );
  osg::Vec3d center ( point[3], point[4], point[5] );
  osg::Vec3d up     ( point[6], point[7], point[8] );

  // Make sure the vectors are normalized.
  up.normalize();

  // Make the matrix.
  osg::Matrixd mat ( osg::Matrixd::identity() );
  mat.makeLookAt ( eye, center, up );

  // Set the matrix.
  vm->setViewMatrix ( mat );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the player.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::update ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Return now if we're not playing.
  if ( false == this->playing() )
    return;

  // If the curve is bad, stop playing and return.
  if ( false == _curve.valid() )
  {
    this->stopPlaying ( caller );
    return;
  }

  // Increment the current parameter.
  _current += this->stepSize();

  // Check to see if we're off the end.
  if ( _current > _curve.lastKnot() )
  {
    // Set current parameter to the end.
    _current = _curve.lastKnot();

    // Make sure we don't continue.
    this->playing ( false );
  }

  // Go to the parametric position.
  this->go ( _current, caller );
}
