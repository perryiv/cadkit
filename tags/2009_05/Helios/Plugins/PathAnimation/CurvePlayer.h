
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

#ifndef _PATH_ANIMATION_CURVE_PLAYER_H_
#define _PATH_ANIMATION_CURVE_PLAYER_H_

#include "PathAnimation/CompileGuard.h"

#include "GN/Config/UsulConfig.h"
#include "GN/Splines/Curve.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IUnknown.h"

#include <stdexcept>

namespace osg { class Node; }

class CameraPath;


class CurvePlayer : public Usul::Base::Object
{
public:

  // Smart pointers.
  USUL_DECLARE_QUERY_POINTERS ( CurvePlayer );

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Errors::ThrowingPolicy < std::runtime_error > ErrorChecker;
  typedef GN::Config::UsulConfig < double, double, unsigned int, ErrorChecker > Config;
  typedef GN::Splines::Curve < Config > Curve;
  typedef Curve::IndependentSequence IndependentSequence;
  typedef Curve::DependentContainer DependentContainer;
  typedef Curve::DependentSequence DependentSequence;
  typedef Curve::IndependentType Parameter;
  typedef std::pair < Curve, IndependentSequence > CurveData;

  // Constructor.
  CurvePlayer();

  // Build a curve.
  static osg::Node *            buildCurve ( const CameraPath *, unsigned int degree, unsigned int steps, IUnknown::RefPtr caller );

  // Clear the player.
  void                          clear();

  // Go to the current parameter.
  void                          go ( Parameter u, IUnknown::RefPtr caller );

  // Interpolate the path.
  static void                   interpolate ( const CameraPath *, unsigned int degree, bool reverse, Curve &curve, IndependentSequence &params );

  // Set/get the flag that says to loop.
  void                          looping ( bool );
  bool                          looping() const;

  // Play the path from the current paramater.
  void                          playBackward ( const CameraPath *, unsigned int degree, IUnknown::RefPtr caller );
  void                          playForward  ( const CameraPath *, unsigned int degree, IUnknown::RefPtr caller );

  // Get/set flag.
  void                          playing ( bool );
  bool                          playing() const;

  // Set/get the number of steps per knot span.
  void                          numStepsPerSpan ( unsigned int num );
  unsigned int                  numStepsPerSpan() const;

  // Update the player.
  void                          update ( IUnknown::RefPtr caller );

protected:

  // Use reference counting.
  virtual ~CurvePlayer();

  static void                   _makePathParams ( const CurveData &, unsigned int stepsPerSpan, IndependentSequence & );
  void                          _makePathParams();

  void                          _notifyStarted();
  void                          _notifyStep ( unsigned int step, unsigned int totalSteps );
  void                          _notifyStopped();

  void                          _play ( const CameraPath *, unsigned int degree, IUnknown::RefPtr caller, bool reverse );

private:

  bool _playing;
  CurveData _curve;
  IndependentSequence _pathParams;
  unsigned int _currentStep;
  unsigned int _stepsPerSpan;
  bool _looping;
  IUnknown::RefPtr _caller;
};


#endif // _PATH_ANIMATION_CURVE_PLAYER_H_
