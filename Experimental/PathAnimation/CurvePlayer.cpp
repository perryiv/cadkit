
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

#include "Usul/Interfaces/IRenderLoop.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Scope/Reset.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Math/Transpose.h"
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
  _current ( 0 ),
  _steps ( 100 ),
  _renderLoop ( false ),
  _looping ( false )
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
//  Set the step size for each knot span.
//
///////////////////////////////////////////////////////////////////////////////

void CurvePlayer::numStepsPerSpan ( unsigned int num )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _steps = num;
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
  return _steps;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the total number of steps.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int CurvePlayer::numStepsTotal ( const Curve &curve, unsigned int stepsPerSpan )
{
  USUL_TRACE_SCOPE_STATIC;

  // Note: totalNumControlPoints() should not throw but return 0 if the curve is empty.
  return ( ( 0 == curve.totalNumControlPoints() ) ? 0 : ( stepsPerSpan * ( curve.totalNumControlPoints() - 1 ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the total number of steps.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int CurvePlayer::numStepsTotal() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return CurvePlayer::numStepsTotal ( _curve, this->numStepsPerSpan() );
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

  // Set current step.
  _current = 0;

  // Initialize.
  this->playing ( false );

  // Try to make the curve.
  IndependentSequence params;
  CurvePlayer::interpolate ( path, degree, reverseOrder, _curve, params );

  // Did it work?
  if ( false == _curve.valid() )
    return;

  // We are now playing.
  this->playing ( true );

  // Check to see if the caller supports render loop.
  Usul::Interfaces::IRenderLoop::QueryPtr rl ( caller );
  if ( true == rl.valid() )
  {
    // Save current render-loop state and turn it on.
    _renderLoop = rl->renderLoop();
    rl->renderLoop ( true );
  }
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

  // Determine total number of steps.
  const unsigned int totalSteps ( this->numStepsTotal() );

  // Check to see if we're off the end.
  if ( _current >= totalSteps )
  {
    // Are we supposed to loop?
    if ( false == this->looping() )
    {
      // No longer playing.
      this->playing ( false );
      return;
    }

    // We're looping so reset the step.
    _current = 0;
  }

  // Go to the parametric position.
  const Parameter u ( static_cast<Parameter> ( _current ) / static_cast<Parameter> ( totalSteps - 1 ) );
  this->go ( u, caller );

  // Increment the current step.
  ++_current;
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
    cube->addDrawable ( OsgTools::ShapeFactorySingleton::instance().cube ( osg::Vec3 ( size, size, size ) ) );

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

osg::Node *CurvePlayer::buildCurve ( const CameraPath *path, unsigned int degree, unsigned int steps, Usul::Interfaces::IUnknown * )
{
  USUL_TRACE_SCOPE;

  // Make new group to hold everything.
  osg::ref_ptr<osg::Group> group ( new osg::Group );

  // Is there a valid path?
  if ( ( 0x0 == path ) || ( path->size() < 2 ) )
    return group.release();

  // Try to make the curve.
  Curve curve;
  IndependentSequence params;
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
    // Determine total number of steps.
    const unsigned int totalSteps ( CurvePlayer::numStepsTotal ( curve, steps ) );

    // Loop through the all steps.
    for ( unsigned int i = 0; i < totalSteps; ++i )
    {
      // Determine parameter.
      const Parameter u ( static_cast<Parameter> ( i ) / static_cast<Parameter> ( totalSteps - 1 ) );

      // Evaluate the curve.
      Curve::Vector point ( curve.dimension() );
      GN::Evaluate::point ( curve, u, point );
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
