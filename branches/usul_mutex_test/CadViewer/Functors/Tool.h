
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all tools.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_TOOL_FUNCTOR_H_
#define _CV_TOOL_FUNCTOR_H_

#include "Usul/Functors/Interaction/Input/AnalogInput.h"
#include "Usul/Functors/Interaction/Navigate/Direction.h"

#include "Usul/Interfaces/IFrameInfo.h"
#include "Usul/Interfaces/IWorldInfo.h"
#include "Usul/Math/Matrix44.h"

#include "osg/MatrixTransform"

#include <vector>


namespace CV {
namespace Functors {


class Tool : public Usul::Functors::Interaction::Common::BaseFunctor
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( Tool );
  typedef Usul::Functors::Interaction::Common::BaseFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Math::Matrix44f Matrix;
  typedef Usul::Functors::Interaction::Input::AnalogInput AnalogInput;
  typedef Usul::Functors::Interaction::Navigate::Direction Direction;
  typedef Direction::Vector Vector;
  typedef Usul::Interfaces::IFrameInfo FrameInfo;
  typedef Usul::Interfaces::IWorldInfo WorldInfo;
  typedef osg::MatrixTransform Transform;
  typedef USUL_REF_POINTER(Transform) TransformPtr;
  typedef std::vector<TransformPtr> Transforms;

  // Direct access to the transforms.
  const Transforms &    transforms() const;
  Transforms &          transforms();
  void                  transforms ( const Transforms &t );

protected:

  Tool ( Unknown *unknown, Direction *di, AnalogInput *ai, const std::string& name );
  Tool ( const Tool &cb );
  virtual ~Tool();

  // Get the analog input.
  float                 _analog();

  // Get the direction vector.
  const Vector &        _direction();

  // Get the amount of time the last frame took.
  float                 _frameTime() const;

  // Post or pre-multiply the current matrix.
  void                  _postMult ( const Matrix & );
  void                  _preMult  ( const Matrix & );

  // Get the radius of the "world".
  float                 _worldRadius() const;

private:

  Direction::RefPtr        _di;
  Vector                   _dir;
  AnalogInput::ValidRefPtr _ai;
  FrameInfo::ValidQueryPtr _fi;
  WorldInfo::ValidQueryPtr _wi;
  Transforms _mt;
};


}; // namespace Functors
}; // namespace CV


#endif // _CV_TRANSLATION_FUNCTORS_H_
