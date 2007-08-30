
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_ANIMATE_PATH_H__
#define __VRV_ANIMATE_PATH_H__

#include "VRV/Export.h"
#include "VRV/Animate/Frame.h"

#include "Usul/Base/Object.h"

#include "Serialize/XML/Macros.h"

#include "GN/Config/UsulConfig.h"
#include "GN/Splines/Curve.h"
#include "GN/Evaluate/Point.h"
#include "GN/Write/XML.h"

#include <vector>

namespace Usul { namespace Interfaces { struct IUnknown; } }

namespace VRV {
namespace Animate {

class VRV_EXPORT Path : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object                                      BaseClass;
  typedef std::vector < Frame::RefPtr >                           Frames;
  typedef GN::Config::UsulConfig < double, double, unsigned int > DoubleConfig;
  typedef GN::Splines::Curve < DoubleConfig >                     DoubleCurve;
  typedef DoubleCurve::IndependentSequence                        IndependentSequence;
  typedef DoubleCurve::DependentContainer                         DependentContainer;
  typedef DoubleCurve::DependentSequence                          DependentSequence;
  typedef DoubleCurve::IndependentType                            Parameter;

  USUL_DECLARE_REF_POINTERS ( Path );

  Path ();

  /// Append a frame.
  void              append ( Frame *frame );

  /// Clear.
  void              clear ();

  /// Get/Set the dirty flag.
  void              dirty ( bool );
  bool              dirty () const;

  /// Start the animation from beginning.
  void              start ( Usul::Interfaces::IUnknown * caller );

  /// Animate one step.
  void              animate ( Usul::Interfaces::IUnknown * caller );

  /// Are we animating?
  bool              animating () const;
protected:
  virtual ~Path ();

  void              _interpolate ( );

private:
  bool                _dirty;
  Frames              _frames;
  unsigned int        _numberSteps;
  unsigned int        _currentStep;
  //Parameter           _segTime;
  //Parameter           _lastU;
  IndependentSequence _params;
  DependentContainer  _rotations;
  DoubleCurve         _curve;
  double              _startTime;
  bool                _animating;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( Path );
};

}
}


#endif // __VRV_ANIMATE_PATH_H__
