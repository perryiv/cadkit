
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

  // Constructor.
  CurvePlayer();

  // Get/Set flag.
  void                          playing ( bool );
  bool                          playing() const;

  // Clear the player.
  void                          clear();

  // Go to the current parameter.
  void                          go ( Parameter u, IUnknown *caller );

  // Play the path from the current paramater.
  void                          playBackward ( const CameraPath *, unsigned int degree, IUnknown *caller );
  void                          playForward  ( const CameraPath *, unsigned int degree, IUnknown *caller );

  // Stop the animation.
  void                          stopPlaying ( IUnknown *caller );

  // Set/get the step size.
  void                          stepSize ( Parameter delta );
  Parameter                     stepSize() const;

  // Update the player.
  void                          update ( IUnknown *caller );

protected:

  // Use reference counting.
  virtual ~CurvePlayer();

  void                          _interpolate ( const CameraPath *, unsigned int degree, bool reverse );

  void                          _play ( const CameraPath *, unsigned int degree, IUnknown *caller, bool reverse );

private:

  bool _playing;
  Curve _curve;
  Parameter _current;
  Parameter _step;
  bool _renderLoop;
};


#endif // _PATH_ANIMATION_CURVE_PLAYER_H_