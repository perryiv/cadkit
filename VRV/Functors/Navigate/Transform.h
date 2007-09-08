
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base callback class for functors that transform the scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_TRANFORM_FUNCTORS_H_
#define _VRV_TRANFORM_FUNCTORS_H_

#include "VRV/Export.h"
#include "VRV/Functors/Input/AnalogInput.h"
#include "VRV/Functors/Navigate/Direction.h"

#include "Usul/Interfaces/IFrameInfo.h"
#include "Usul/Interfaces/IWorldInfo.h"
#include "Usul/Interfaces/IMatrixMultiply.h"
#include "Usul/Math/Matrix44.h"


namespace VRV {
namespace Functors {


class VRV_EXPORT Transform : public VRV::Functors::BaseFunctor
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( Transform );
  typedef VRV::Functors::BaseFunctor BaseClass;

protected:

  typedef BaseClass::Unknown Unknown;
  typedef VRV::Functors::Direction Direction;
  typedef VRV::Functors::AnalogInput AnalogInput;
  typedef Direction::Vector Vector;
  typedef Usul::Math::Matrix44f Matrix;
  typedef Usul::Interfaces::IFrameInfo FrameInfo;
  typedef Usul::Interfaces::IWorldInfo WorldInfo;
  typedef Usul::Interfaces::IMatrixMultiplyFloat MatrixMultiply;

  Transform (
    Unknown *unknown,
    Direction *dir,
    AnalogInput *ai,
    unsigned int id );
  Transform ( const Transform &cb );
  virtual ~Transform();

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

  Direction::ValidRefPtr _dir;
  AnalogInput::ValidRefPtr _ai;
  FrameInfo::ValidQueryPtr _fi;
  WorldInfo::ValidQueryPtr _wi;
  MatrixMultiply::ValidQueryPtr _mm;
};


} // namespace Functors
} // namespace VRV


#endif // _VRV_TRANFORM_FUNCTORS_H_
