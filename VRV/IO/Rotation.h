
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that encapsulates rotation information.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_IO_ROTATION_CLASS_H_
#define _VRV_IO_ROTATION_CLASS_H_

#include "VRV/IO/CompileGuard.h"

#include "Usul/Math/Vector3.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Constants.h"


namespace VRV {
namespace IO {
namespace Detail {


template < class RealType_ > class Rotation
{
public:

  // Typedefs.
  typedef RealType_ RealType;
  typedef Usul::Math::Vector3<RealType> Vec3;
  typedef Usul::Math::Matrix44<RealType> Matrix;

  // Default construction.
  Rotation() : _radians ( 0 ), _axis ( 1, 0, 0 )
  {
  }

  // Reset the members to their defaults.
  void reset()
  {
    _radians = 0;
    _axis.set ( 1, 0, 0 );
  }

  // Indicates that the start of a model has been encountered.
  void modelStart()
  {
    this->reset();
  }

  // Indicates that the end of a model has been encountered.
  void modelFinish()
  {
    this->reset();
  }

  // Get the angle in radians.
  RealType radians() const
  {
    return _radians;
  }

  // Set the angle in radians.
  void radians ( RealType r )
  {
    _radians = r;
  }

  // Set the angle in degrees.
  void degrees ( RealType d )
  {
    this->radians ( Usul::Math::DEG_TO_RAD * d );
  }

  // Get the axis.
  const Vec3 &axis() const
  {
    return _axis;
  }

  // Set the axis.
  void axis ( const Vec3 &axis )
  {
    _axis = axis;
    _axis.normalize();
  }

  // Get a rotation matrix.
  void matrix ( Matrix &m ) const
  {
    m.makeRotation ( this->radians(), this->axis() );
  }

private:

  // No copy construction or assignment.
  Rotation ( const Rotation & );
  Rotation &operator = ( const Rotation & );

  RealType _radians;
  Vec3 _axis;
};


}; // namespace Detail
}; // namespace OSG
}; // namespace VRV


#endif // _VRV_IO_ROTATION_CLASS_H_
