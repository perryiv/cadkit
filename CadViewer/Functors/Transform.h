
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

#ifndef _CV_TRANFORM_FUNCTORS_H_
#define _CV_TRANFORM_FUNCTORS_H_

#include "CadViewer/Functors/AnalogInput.h"
#include "CadViewer/Functors/Direction.h"

#include "VRV/Interfaces/IFrameInfo.h"
#include "VRV/Interfaces/IWorldInfo.h"
#include "VRV/Interfaces/IMatrixMultiply.h"

#include "Usul/Math/Matrix44.h"


namespace CV {
namespace Functors {


class Transform : public VRV::Functors::BaseFunctor
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( Transform );
  typedef VRV::Functors::BaseFunctor BaseClass;

protected:

  typedef BaseClass::Unknown Unknown;
  typedef CV::Functors::Direction Direction;
  typedef CV::Functors::AnalogInput AnalogInput;
  typedef Direction::Vector Vector;
  typedef Usul::Math::Matrix44f Matrix;
  typedef VRV::Interfaces::IFrameInfo FrameInfo;
  typedef VRV::Interfaces::IWorldInfo WorldInfo;
  typedef VRV::Interfaces::IMatrixMultiplyFloat MatrixMultiply;

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


}; // namespace Functors
}; // namespace CV


#endif // _CV_TRANFORM_FUNCTORS_H_
