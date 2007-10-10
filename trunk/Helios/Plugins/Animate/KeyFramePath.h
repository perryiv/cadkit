
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_ANIMATE_KEY_FRAME_PATH_H__
#define __VRV_ANIMATE_KEY_FRAME_PATH_H__

#include "Helios/Plugins/Animate/Path.h"
#include "Helios/Plugins/Animate/Frame.h"

#include "Usul/Base/Object.h"

#include "Serialize/XML/Macros.h"

#include "GN/Config/UsulConfig.h"
#include "GN/Splines/Curve.h"
#include "GN/Evaluate/Point.h"
#include "GN/Write/XML.h"

#include <vector>

namespace Usul { namespace Interfaces { struct IUnknown; } }

namespace Animate {

class KeyFramePath : public Animate::Path
{
public:
  typedef Animate::Path                                          BaseClass;
  typedef std::vector < Frame::RefPtr >                           Frames;
  typedef GN::Config::UsulConfig < double, double, unsigned int > DoubleConfig;
  typedef GN::Splines::Curve < DoubleConfig >                     DoubleCurve;
  typedef DoubleCurve::IndependentSequence                        IndependentSequence;
  typedef DoubleCurve::DependentContainer                         DependentContainer;
  typedef DoubleCurve::DependentSequence                          DependentSequence;
  typedef DoubleCurve::IndependentType                            Parameter;

  USUL_DECLARE_REF_POINTERS ( KeyFramePath );

  KeyFramePath ();

  /// Clear.
  virtual void      clear ( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Start the animation from beginning.
  virtual void      start ( Usul::Interfaces::IUnknown * caller );

  /// Animate one step.
  virtual void      updateNotify ( Usul::Interfaces::IUnknown * caller );

  /// Get/Set the number of animation steps.
  void              steps ( unsigned int value );
  unsigned int      steps ( ) const;

  /// Get the extension for the file.
  virtual std::string  extension () const;

  /// Get the name of the path.
  virtual std::string  name () const;

protected:
  virtual ~KeyFramePath ();

  void              _interpolate ( );

  /// Append a frame.
  virtual void      _append ( Frame* frame );

private:
  Frames              _frames;
  IndependentSequence _params;
  DependentContainer  _rotations;
  DoubleCurve         _curve;
  unsigned int        _numberSteps;
  unsigned int        _currentStep;

  SERIALIZE_XML_DEFINE_MEMBERS ( KeyFramePath );
};


}


#endif // __VRV_ANIMATE_PATH_H__
