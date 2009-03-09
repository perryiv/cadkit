
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
#include "GN/Tessellate/Bisect.h"

#include "OsgTools/State/StateSet.h"
#include "OsgTools/ShapeFactory.h"

#include "Usul/Interfaces/IAnimationNotify.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Scope/Reset.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Math/Transpose.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "osg/AutoTransform"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Matrixd"
#include "osg/MatrixTransform"
#include "osg/ref_ptr"
#include "osg/Vec3d"
#include <algorithm>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CurvePlayer::CurvePlayer() : BaseClass(),
  _playing ( false ),
  _curve(),
  _pathParams(),
  _currentStep ( 0 ),
  _stepsPerSpan ( 100 ),
  _looping ( false ),
  _caller ( 0x0 )
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
  _caller = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the step size for each knot span.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::numStepsPerSpan ( unsigned int num )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _stepsPerSpan = num;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the step size for each knot span.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int CurvePlayer::numStepsPerSpan() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _stepsPerSpan;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play the animation from the current parameter.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::_play ( const CameraPath *path, unsigned int degree, IUnknown::RefPtr caller, bool reverseOrder )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Set current step.
  _currentStep = 0;

  // Save caller.
  _caller = caller;

  // Initialize.
  this->playing ( false );

  // Try to make the curve.
  CurvePlayer::interpolate ( path, degree, reverseOrder, _curve.first, _curve.second );
  if ( false == _curve.first.valid() )
    return;

  // Make the path's parameters.
  this->_makePathParams();
  if ( true == _pathParams.empty() )
    return;

  // We are now playing.
  this->playing ( true );

  // Notify the caller.
  this->_notifyStarted();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play the animation forward from the current parameter.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::playForward ( const CameraPath *path, unsigned int degree, IUnknown::RefPtr caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  this->_play ( path, degree, caller, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play the animation backward from the current parameter.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::playBackward ( const CameraPath *path, unsigned int degree, IUnknown::RefPtr caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  this->_play ( path, degree, caller, true );
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
  _curve.first.clear();
  _curve.second.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Interpolate the path.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::interpolate ( const CameraPath *path, unsigned int degree, bool reverseOrder, Curve &curve, IndependentSequence &params )
{
  USUL_TRACE_SCOPE_STATIC;

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
  curve.clear();

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
  params.clear();
  params.reserve ( values.size() );
  Parameterize::fit ( eyes, GN::Algorithms::Constants::CENTRIPETAL_FIT, params );

  // Make the knot vector. Size it for interpolation.
  const SizeType order ( Usul::Math::minimum<SizeType> ( degree + 1, params.size() ) );
  IndependentSequence knots;
  knots.resize ( params.size() + order );
  KnotVectorBuilder::build ( params, order, knots );

  // Interpolate the points.
  GN::Interpolate::global ( order, params, knots, points, curve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go to the current parameter.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::go ( Parameter u, IUnknown::RefPtr caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Get the needed interface.
  Usul::Interfaces::IViewMatrix::QueryPtr vm ( caller );
  if ( false == vm.valid() )
    return;

  // Make sure we have a valid curve.
  if ( false == _curve.first.valid() )
    return;

  // Make sure the parameter is in range.
  if ( ( u < _curve.first.firstKnot() ) || ( u > _curve.first.lastKnot() ) )
    return;

  // Check number of dependent variables.
  if ( 9 != _curve.first.numDepVars() )
    return;

  // Evaluate the dependent variables. Have to size the point!
  Curve::Vector point ( _curve.first.numDepVars() );
  GN::Evaluate::point ( _curve.first, u, point );

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

void CurvePlayer::update ( IUnknown::RefPtr caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Return now if we're not playing.
  if ( ( false == this->playing() ) || ( true == _pathParams.empty() ) )
    return;

  // If the curve is bad, stop playing and return.
  if ( false == _curve.first.valid() )
  {
    this->playing ( false );
    this->_notifyStopped();
    return;
  }

  // Check to see if we're off the end.
  if ( _currentStep >= _pathParams.size() )
  {
    // Are we supposed to loop?
    if ( false == this->looping() )
    {
      // No longer playing.
      this->playing ( false );
      this->_notifyStopped();
      return;
    }

    // We're looping so reset the step.
    _currentStep = 0;
  }

  // Determine parameter.
  const Parameter u ( _pathParams.at ( _currentStep ) );

  // Feedback.
  if ( ( _currentStep > 0 ) && ( 0 == ( _currentStep % 100 ) ) )
  {
    std::cout << Usul::Strings::format ( "Rendering step ", _currentStep, " of ", _pathParams.size() ) << std::endl;
  }

  // Go to the parametric position.
  this->go ( u, caller );

  // Notify the caller.
  this->_notifyStep ( _currentStep, _pathParams.size() );

  // Increment the current step.
  ++_currentStep;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the path's parameters.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::_makePathParams()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  CurvePlayer::_makePathParams ( _curve, this->numStepsPerSpan(), _pathParams );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the path's parameters.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::_makePathParams ( const CurveData &curve, unsigned int stepsPerSpan, IndependentSequence &pathParams )
{
  USUL_TRACE_SCOPE_STATIC;

  // Initialize.
  pathParams.clear();

  // Check input.
  if ( 0 == stepsPerSpan )
    return;

  // Note: totalNumControlPoints() should not throw but return 0 if the curve is empty.
  const unsigned int numControlPoints ( curve.first.totalNumControlPoints() );
  if ( numControlPoints < 2 )
    return;

  // Reserve space.
  pathParams.reserve ( stepsPerSpan * numControlPoints );

  // Loop through the spans.
  const unsigned int numSpans ( numControlPoints - 1 );
  for ( unsigned int s = 0; s < numSpans; ++s )
  {
    // The parameters to stay between.
    const Parameter u0 ( curve.second.at ( s ) );
    const Parameter u1 ( curve.second.at ( s + 1 ) );

    // Loop through the steps.
    for ( unsigned int i = 0; i < stepsPerSpan; ++i )
    {
      // Determine parameter.
      Parameter u ( static_cast<Parameter> ( i ) / static_cast<Parameter> ( stepsPerSpan - 1 ) );
      u = u0 + ( u * ( u1 - u0 ) );

      // Ensure last parameter is exactly u1.
      u = ( ( i + 1 == stepsPerSpan ) ? u1 : u );

      // Add the parameter.
      pathParams.push_back ( u );
    }
  }

  // Have to remove non-unique parameters.
  std::sort ( pathParams.begin(), pathParams.end() );
  pathParams.erase ( std::unique ( pathParams.begin(), pathParams.end() ), pathParams.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we looping?
//
///////////////////////////////////////////////////////////////////////////////

bool CurvePlayer::looping() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _looping;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we looping?
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::looping ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _looping = state;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to make a cube.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class Point > osg::Node *makeCube ( const Point &point, float size )
  {
    osg::ref_ptr<osg::Geode> cube ( new osg::Geode );
    cube->addDrawable ( OsgTools::ShapeFactory::instance().cube ( osg::Vec3 ( size, size, size ) ) );

    osg::ref_ptr<osg::AutoTransform> mat ( new osg::AutoTransform );
    mat->setAutoRotateMode ( osg::AutoTransform::NO_ROTATION );
    mat->setAutoScaleToScreen ( true );
    mat->setPosition ( osg::Vec3 ( point.at(0), point.at(1), point.at(2) ) );
    mat->addChild ( cube.get() );

    OsgTools::State::StateSet::setBackFaceCulling ( cube->getOrCreateStateSet(), true );
    OsgTools::State::StateSet::setMaterialDefault ( cube.get() );
    OsgTools::State::StateSet::setLighting ( cube.get(), true );
    OsgTools::State::StateSet::setNormalize ( cube.get(), true );

    return mat.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to make axes.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class Point > osg::Node *makeAxes ( const Point &point, float length )
  {
    osg::ref_ptr<osg::AutoTransform> transform ( new osg::AutoTransform );
    transform->setAutoRotateMode ( osg::AutoTransform::NO_ROTATION );
    transform->setAutoScaleToScreen ( true );

    if ( point.size() < 9 )
      return transform.release();

    osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
    transform->addChild ( geode.get() );

    osg::ref_ptr<osg::Geometry> geom ( new osg::Geometry );
    geode->addDrawable ( geom.get() );

    const osg::Vec3Array::value_type origin ( 0, 0, 0 );
    const osg::Vec3Array::value_type eye    ( point.at(0), point.at(1), point.at(2) );
    const osg::Vec3Array::value_type center ( point.at(3), point.at(4), point.at(5) );
    osg::Vec3Array::value_type up           ( point.at(6), point.at(7), point.at(8) );
    up.normalize();

    osg::Vec3Array::value_type lookat  ( center - eye );
    osg::Vec3Array::value_type tangent ( lookat ^ up );

    lookat.normalize();
    tangent.normalize();

    transform->setPosition ( eye );

    lookat *= length;
    up *= length;
    tangent *= length;

    osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
    vertices->reserve ( 6 );
    vertices->push_back ( origin );
    vertices->push_back ( origin + lookat );
    vertices->push_back ( origin );
    vertices->push_back ( origin + up );
    vertices->push_back ( origin );
    vertices->push_back ( origin + tangent );
    geom->setVertexArray ( vertices.get() );

    osg::ref_ptr<osg::Vec4Array> colors ( new osg::Vec4Array );
    colors->reserve ( 6 );
    colors->push_back ( osg::Vec4Array::value_type ( 1, 0, 0, 1 ) );
    colors->push_back ( osg::Vec4Array::value_type ( 1, 0, 0, 1 ) );
    colors->push_back ( osg::Vec4Array::value_type ( 0, 1, 0, 1 ) );
    colors->push_back ( osg::Vec4Array::value_type ( 0, 1, 0, 1 ) );
    colors->push_back ( osg::Vec4Array::value_type ( 0, 0, 1, 1 ) );
    colors->push_back ( osg::Vec4Array::value_type ( 0, 0, 1, 1 ) );
    geom->setColorArray ( colors.get() );
    geom->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );

    geom->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINES, 0, vertices->size() ) );

    OsgTools::State::StateSet::setLighting ( transform.get(), false );

    return transform.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the curve.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *CurvePlayer::buildCurve ( const CameraPath *path, unsigned int degree, unsigned int steps, IUnknown::RefPtr )
{
  USUL_TRACE_SCOPE_STATIC;

  // Make new group to hold everything.
  osg::ref_ptr<osg::Group> group ( new osg::Group );

  // Check input.
  if ( ( steps < 2 ) || ( degree < 1 ) )
    return group.release();

  // Is there a valid path?
  if ( ( 0x0 == path ) || ( path->size() < 2 ) )
    return group.release();

  // Try to make the curve.
  CurveData data;
  Curve &curve ( data.first );
  IndependentSequence &params ( data.second );
  CurvePlayer::interpolate ( path, degree, false, curve, params );

  // Did it work?
  if ( ( false == curve.valid() ) || ( curve.dimension() < 3 ) )
    return group.release();

  // Make the curve.
  {
    // Bisect the curve.
    Curve::IndependentSequence u;
    const Curve::DependentArgument chordHeight ( 0.01 );
    GN::Tessellate::bisect ( curve, chordHeight, u );

    // Make vertices and colors.
    osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
    osg::ref_ptr<osg::Vec4Array> colors ( new osg::Vec4Array );
    vertices->reserve ( u.size() );
    colors->reserve ( u.size() );

    // Used in the loop.
    bool toggle ( true );
    const osg::Vec4Array::value_type color1 ( 0, 0, 0, 1 );
    const osg::Vec4Array::value_type color2 ( 1, 1, 1, 1 );

    // Loop through the parameters.
    for ( IndependentSequence::const_iterator i = u.begin(); i != u.end(); ++i )
    {
      Curve::Vector point ( curve.dimension() );
      GN::Evaluate::point ( curve, *i, point );
      if ( point.size() >= 3 )
      {
        // Add vertex.
        vertices->push_back ( osg::Vec3Array::value_type ( point.at(0), point.at(1), point.at(2) ) );

        // Every other color switches.
        colors->push_back ( ( toggle ) ? color1 : color2 );
        toggle = !toggle;
      }
    }

    // Make geometry.
    osg::ref_ptr<osg::Geometry> geom ( new osg::Geometry );
    geom->setVertexArray ( vertices.get() );
    geom->setColorArray ( colors.get() );
    geom->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );

    // It's a line-set.
    geom->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINE_STRIP, 0, vertices->size() ) );

    // Add to geode.
    osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
    geode->addDrawable ( geom.get() );
    group->addChild ( geode.get() );

    // Set state.
    OsgTools::State::StateSet::setLighting ( geode.get(), false );
  }

  // Make the axes at each position.
  {
    // Loop through the parameters.
    for ( IndependentSequence::const_iterator i = params.begin(); i != params.end(); ++i )
    {
      Curve::Vector point ( curve.dimension() );
      GN::Evaluate::point ( curve, *i, point );
      if ( point.size() >= 9 )
      {
        // Add axes.
        group->addChild ( Helper::makeAxes ( point, 50.0f ) );

        // Add cube at the center.
        group->addChild ( Helper::makeCube ( point, 10.0f ) );
      }
    }
  }

  // Make smaller axes at each evaluated position.
  {
    // Make the parameters.
    IndependentSequence pathParams;
    CurvePlayer::_makePathParams ( data, steps, pathParams );

    // Loop through the parameters.
    for ( IndependentSequence::const_iterator i = pathParams.begin(); i != pathParams.end(); ++i )
    {
      // Evaluate the curve.
      Curve::Vector point ( curve.dimension() );
      GN::Evaluate::point ( curve, *i, point );
      if ( point.size() >= 3 )
      {
        // Add axes.
        group->addChild ( Helper::makeAxes ( point, 25.0f ) );
      }
    }
  }

  // Set state.
  OsgTools::State::StateSet::setLineWidth ( group.get(), 2.0f );

  // Return group.
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the caller.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::_notifyStarted()
{
  // Notify the caller.
  typedef Usul::Interfaces::IAnimationNotify IAnimationNotify;
  IAnimationNotify::QueryPtr notify ( Usul::Threads::Safe::get ( this->mutex(), _caller.get() ) );
  if ( true == notify.valid() )
  {
    notify->animationStarted();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the caller.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::_notifyStep ( unsigned int step, unsigned int totalSteps )
{
  // Notify the caller.
  typedef Usul::Interfaces::IAnimationNotify IAnimationNotify;
  IAnimationNotify::QueryPtr notify ( Usul::Threads::Safe::get ( this->mutex(), _caller.get() ) );
  if ( true == notify.valid() )
  {
    notify->animationStep ( step, totalSteps );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the caller.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::_notifyStopped()
{
  // Notify the caller.
  typedef Usul::Interfaces::IAnimationNotify IAnimationNotify;
  IAnimationNotify::QueryPtr notify ( Usul::Threads::Safe::get ( this->mutex(), _caller.get() ) );
  if ( true == notify.valid() )
  {
    notify->animationStopped();
  }
}
