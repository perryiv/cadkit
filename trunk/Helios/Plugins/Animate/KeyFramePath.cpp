
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/Animate/KeyFramePath.h"

#include "GN/Interpolate/Global.h"
#include "GN/Algorithms/KnotVector.h"
#include "GN/Algorithms/Fill.h"
#include "GN/Algorithms/Parameterize.h"
#include "GN/Tessellate/Bisect.h"

#include "Usul/Math/Transpose.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Factory/RegisterCreator.h"

using namespace Animate;

USUL_FACTORY_REGISTER_CREATOR_WITH_NAME ( "KeyFramePath", KeyFramePath );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

KeyFramePath::KeyFramePath () : 
  BaseClass ( "Key Frame Path" ),
  _frames (),
  _params (),
  _rotations (),
  _curve (),
  _numberSteps ( 50 ),
  _currentStep ( 0 )
{
  this->_addMember ( "Frames", _frames );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

KeyFramePath::~KeyFramePath ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append a frame.
//
///////////////////////////////////////////////////////////////////////////////

void KeyFramePath::_append ( Frame* frame )
{
  {
    Guard guard ( this->mutex () );
    _frames.push_back ( frame );
  }
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation from beginning.
//
///////////////////////////////////////////////////////////////////////////////

void KeyFramePath::start ( Usul::Interfaces::IUnknown * caller )
{
  if ( this->dirty () )
  {
    this->_interpolate ();
  }

  {
    Guard guard ( this->mutex () );
    _currentStep = 0;
  }

  BaseClass::start ( caller );

  this->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate one step.
//
///////////////////////////////////////////////////////////////////////////////

void KeyFramePath::updateNotify ( Usul::Interfaces::IUnknown * caller )
{
  if ( false == this->animating () || _params.empty () || false == _curve.valid() )
    return;

  Usul::Interfaces::IViewMatrix::QueryPtr vm ( caller );

  if ( vm.valid () )
  {
    typedef DoubleCurve::Vector Point;
    typedef DoubleCurve::SizeType SizeType;
    typedef IndependentSequence::iterator Iterator;
    typedef IndependentSequence::const_iterator ConstIterator;
    typedef std::greater<Parameter> IsGreater;

    // Determine total number of steps to take..
    unsigned int totalNumberSteps ( this->steps () * _params.size() );

    // The independent variable.
    Parameter u ( static_cast < double > ( _currentStep ) / static_cast < double > ( totalNumberSteps - 1 ) );

    std::cout << "u: " << u;

    // Make a point for the position and rotation.
    Point pos ( _curve.dimension() );
    DependentSequence r0 ( _rotations.size(), 0 );
    DependentSequence r1 ( _rotations.size(), 0 );

    // Evaluate the point.
    GN::Evaluate::point ( _curve, u, pos );

    // Initialize.
    Iterator iu ( _params.begin() );

    // Find the span in the list of parameters.
    iu = std::find_if ( iu, _params.end(), std::bind2nd ( IsGreater(), u ) );

    // Convert this to an index into the list of rotations.
    const SizeType ir1 ( Usul::Math::minimum ( static_cast < SizeType > ( std::distance ( _params.begin(), iu ) ), static_cast < SizeType > ( _params.size() - 1 ) ) );
    const SizeType ir0 ( ir1 - 1 );

    // Get the parameters.
    const Parameter u0 ( _params.at ( ir0 ) );
    const Parameter u1 ( _params.at ( ir1 ) );

    // Get the quaternions.
    r0 = _rotations.at ( ir0 );
    r1 = _rotations.at ( ir1 );

    // What fraction are we between the adjacent parameters?
    const Parameter fraction ( ( u - u0 ) / ( u1 - u0 ) );

    std::cout << " Fraction: " << fraction << std::endl;

    // Spherical linear interpolation.
    const osg::Quat q0 ( r0.at(0), r0.at(1), r0.at(2), r0.at(3) );
    const osg::Quat q1 ( r1.at(0), r1.at(1), r1.at(2), r1.at(3) );
    osg::Quat quat;
    quat.slerp ( fraction, q0, q1 );

    // Translation.
    const osg::Vec3 center ( pos.at(0), pos.at(1), pos.at( 2 ) );

    // The matrix.
    osg::Matrix m;
    m.postMult ( osg::Matrix::rotate ( quat ) );
    m.postMult ( osg::Matrix::translate ( center ) );

    vm->setViewMatrix ( m );

    _currentStep++;

    if ( _currentStep == totalNumberSteps )
    {
      // Evaluate the point.
      GN::Evaluate::point ( _curve, 1, pos );
      r0 = _rotations.back ();

      // The matrix.
      osg::Matrix m;
      m.postMult ( osg::Matrix::rotate ( osg::Quat ( r0.at(0), r0.at(1), r0.at(2), r0.at(3) ) ) );
      m.postMult ( osg::Matrix::translate ( osg::Vec3 ( pos.at ( 0 ), pos.at ( 1 ), pos.at ( 2 ) ) ) );

      vm->setViewMatrix ( m );

      // We are done animating.
      this->stop ( caller );
    }
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Interpolate.
//
///////////////////////////////////////////////////////////////////////////////

void KeyFramePath::_interpolate ( )
{
  // For readability.
  typedef DoubleCurve::IndependentType IndependentType;
  typedef DoubleCurve::SizeType SizeType;
  typedef DoubleCurve::ErrorCheckerType ErrorCheckerType;
  typedef GN::Algorithms::KnotVector < IndependentSequence, ErrorCheckerType > KnotVectorBuilder;
  typedef GN::Algorithms::Parameterize< IndependentSequence, DependentContainer, DoubleCurve::Power, ErrorCheckerType > Paramerterize;

  // Clear what we have.
  _curve.clear ();
  _params.clear ();
  _rotations.clear ();

  // Container for the data.
  DependentContainer points;

  // Loop through the frames.
  for ( Frames::const_iterator i =_frames.begin(); i != _frames.end(); ++i )
  {
    // Get the frame data.
    const Frame &frame ( *(*i) );

    const osg::Vec3 c ( frame.translation () );
    const osg::Quat r ( frame.rotation () );

    if ( r != osg::Quat ( 0.0, 0.0, 0.0, 0.0 ) ) // TODO, is this needed?
    {
      DependentSequence point;
      point.push_back ( c[0] );
      point.push_back ( c[1] );
      point.push_back ( c[2] );
      point.push_back ( 1.0  );
      point.push_back ( r[0] );
      point.push_back ( r[1] );
      point.push_back ( r[2] );
      point.push_back ( r[3] );
      points.push_back ( point );
    }
  }

  // Make sure there are no adjacent points that are identical.
  typedef DependentContainer::IsEqual IsEqualPoint;
  points.erase ( std::unique ( points.begin(), points.end(), IsEqualPoint() ), points.end() );

  // Now that we have unique frames, make sure there are enough.
  if ( points.size() < DoubleCurve::Limits::MIN_NUM_CTR_PTS )
    return;

  // Transpose so that the first index of "points" is the dimension.
  Usul::Math::transpose ( points );

  // Make the parameters evenly spaced.
  //GN::Algorithms::fill ( _params, points.size(), 0, 1 );
  Paramerterize::fit ( points, GN::Algorithms::Constants::CENTRIPETAL_FIT, _params );

  // This is no longer true with the transpose needed for the centripetal fit.
  // Should be true.
  //USUL_ASSERT ( _params.size() == points.size() );

  // Make the knot vector. Size it for interpolation.
  IndependentSequence knots;
  const SizeType cubic ( 4 );
  const SizeType order ( std::min<SizeType> ( cubic, _params.size() ) );

  std::cout << "Order: " << order << std::endl;

  knots.resize ( _params.size() + order );
  KnotVectorBuilder::build ( _params, order, knots );

  // Make a copy of the points.
  DependentContainer &pos = points;
  _rotations = points;

  // Trim the two containers of data. Note: the 4 is related to the 
  // container dimension (see above), not the order.
  pos.erase ( pos.begin() + 4, pos.end() );
  _rotations.erase ( _rotations.begin(), _rotations.begin() + 4 );

  // Note: rot.at(0) should point to the 4 components of the quaternian.
  Usul::Math::transpose ( _rotations );

  // Interpolate the positions.
  GN::Interpolate::global ( order, _params, knots, pos, _curve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear.
//
///////////////////////////////////////////////////////////////////////////////

void KeyFramePath::clear ( Usul::Interfaces::IUnknown *caller )
{
  {
    Guard guard ( this->mutex () );
    _frames.clear();
    _curve.clear();
    _params.clear();
    _rotations.clear();
  }
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of animation steps.
//
///////////////////////////////////////////////////////////////////////////////

void KeyFramePath::steps ( unsigned int value )
{
  Guard guard ( this->mutex () );
  _numberSteps = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of animation steps.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int KeyFramePath::steps ( ) const
{
  Guard guard ( this->mutex () );
  return _numberSteps;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extension for the file.
//
///////////////////////////////////////////////////////////////////////////////

std::string KeyFramePath::extension () const
{
  return "kfp";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name of the path.
//
///////////////////////////////////////////////////////////////////////////////

std::string KeyFramePath::name () const
{
  return "Key Frame Path";
}
