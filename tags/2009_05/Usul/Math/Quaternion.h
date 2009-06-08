
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Quaternion class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_MATH_QUATERNION_H__
#define __USUL_MATH_QUATERNION_H__

#include "Usul/Math/Functions.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"

namespace Usul {
namespace Math {


template
<
  typename T,
  typename Index_ = unsigned int,
  typename ErrorChecker_ = Usul::Errors::AssertPolicy
>
class Quaternion
{
public:

  typedef Quaternion<T,Index_,ErrorChecker_> ThisType;
  typedef T value_type;
  typedef Index_ size_type;
  typedef ErrorChecker_ ErrorChecker;
  typedef Usul::Math::Vector3<T,Index_,ErrorChecker_> Vector3;
  typedef Usul::Math::Vector4<T,Index_,ErrorChecker_> Vector4;
  typedef Usul::Math::Matrix44<T,Index_,ErrorChecker_> Matrix44;

  /// Constructor.
  Quaternion() : _v ( 0.0, 0.0, 0.0, 1.0 )
  {
  }

  /// Constructor.
  Quaternion( T x, T y, T z, T w ) : _v ( x, y, z, w )
  {
  }

  /// Constructor.
  Quaternion ( const Vector3& axis, T angle ) : _v()
  {
    // Should we normalize the axis, or assume it is?
    _v[0] = axis[0];
	  _v[1] = axis[1];
	  _v[2] = axis[2];

    const double scale ( Usul::Math::sin ( angle * 0.5f ) );

    _v[0] *= scale;
    _v[1] *= scale;
    _v[2] *= scale;
    _v[3] = Usul::Math::cos ( angle * 0.5f);
  }

  /// Constructor.
  // From http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
  Quaternion ( const Matrix44& m ) : _v()
  {
    _v[3] = Usul::Math::sqrt ( 1 + m ( 0, 0 ) + m ( 1, 1 ) + m( 2, 2 ) ) / 2.0;
    _v[0] = ( m ( 2, 1 ) - m ( 1, 2 ) ) / ( 4 * _v[3] );
    _v[1] = ( m ( 0, 2 ) - m ( 2, 0 ) ) / ( 4 * _v[3] );
    _v[2] = ( m ( 1, 0 ) - m ( 0, 1 ) ) / ( 4 * _v[3] );
  }

  /// See: http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/index.htm
  Quaternion ( value_type yaw, value_type pitch, value_type roll )  : _v ( 0.0, 0.0, 0.0, 1.0 )
  {
    const double cy ( Usul::Math::cos ( yaw * 0.5 ) );
		const double cp ( Usul::Math::cos ( pitch * 0.5 ) );
		const double cr ( Usul::Math::cos ( roll * 0.5 ) );
		const double sy ( Usul::Math::sin ( yaw * 0.5 ) );
		const double sp ( Usul::Math::sin ( pitch * 0.5 ) );
		const double sr ( Usul::Math::sin ( roll * 0.5 ) );

		const double qw ( cy*cp*cr + sy*sp*sr );
		const double qx ( sy*cp*cr - cy*sp*sr );
		const double qy ( cy*sp*cr + sy*cp*sr );
		const double qz ( cy*cp*sr - sy*sp*cr );

		_v.set ( qx, qy, qz, qw );
	}

  /// See: http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
	Vector3 toEuler() const
	{
		const double q0 ( _v[3] );
		const double q1 ( _v[0] );
		const double q2 ( _v[1] );
		const double q3 ( _v[2] );

		const double heading ( Usul::Math::atan2 ( 2 * (q2*q3 + q0*q1), (q0*q0 - q1*q1 - q2*q2 + q3*q3)) );
		const double pitch ( Usul::Math::asin ( -2 * (q1*q3 - q0*q2)) );
		const double roll ( Usul::Math::atan2 ( 2 * (q1*q2 + q0*q3), (q0*q0 + q1*q1 - q2*q2 - q3*q3)) );

		return Vector3 ( heading, pitch, roll );
	}

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Bracket operators.
  //
  /////////////////////////////////////////////////////////////////////////////

  T &operator [] ( size_type i )
  {
    return _v[i];
  }
  const T &operator [] ( size_type i ) const
  {
    return _v[i];
  }

  /// Multiply by another quaternion.
  ThisType operator* ( const ThisType& rhs ) const
  {
    // See equation 23 on http://mathworld.wolfram.com/Quaternion.html
    return ThisType( rhs._v[3]*_v[0] + rhs._v[0]*_v[3] + rhs._v[1]*_v[2] - rhs._v[2]*_v[1],
                     rhs._v[3]*_v[1] - rhs._v[0]*_v[2] + rhs._v[1]*_v[3] + rhs._v[2]*_v[0],
                     rhs._v[3]*_v[2] + rhs._v[0]*_v[1] - rhs._v[1]*_v[0] + rhs._v[2]*_v[3],
                     rhs._v[3]*_v[3] - rhs._v[0]*_v[0] - rhs._v[1]*_v[1] - rhs._v[2]*_v[2] );
  }

  /// Divide by a scalar.
  ThisType operator/ ( value_type v ) const
  {
    const value_type denom ( 1.0 / v );
    ThisType quat ( *this );
    quat[0] *= denom;
    quat[1] *= denom;
    quat[2] *= denom;
    quat[3] *= denom;
    return quat;
  }

  /// Return the conjugate.
  ThisType conjugate() const
  {
    return ThisType ( -_v[0], -_v[1], -_v[2], _v[3] );
  }

  /// Return the inverse.
  ThisType inverse() const
  {
    return this->conjugate() / this->length2();
  }

  /// Return the length.
  size_type length() const
  {
    return Usul::Math::sqrt ( _v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2] + _v[3] * _v[3] );
  }

  /// Return the length before applying square root.
  size_type length2() const
  {
    return _v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2] + _v[3] * _v[3];
  }

  /// Get a Matrix.
  void getMatrix ( Matrix44& R ) const
  {
    R[0] = 1.0f  - 2.0f * (_v[1] * _v[1] + _v[2] * _v[2]);
    R[1] = 2.0f         * (_v[0] * _v[1] - _v[2] * _v[3]);
    R[2] = 2.0f         * (_v[2] * _v[0] + _v[1] * _v[3]);
    R[3] = 0.0f;

    R[4] = 2.0f         * (_v[0] * _v[1] + _v[2] * _v[3]);
    R[5] = 1.0f  - 2.0f * (_v[2] * _v[2] + _v[0] * _v[0]);
    R[6] = 2.0f         * (_v[1] * _v[2] - _v[0] * _v[3]);
    R[7] = 0.0f;

    R[8] = 2.0f         * (_v[2] * _v[0] - _v[1] * _v[3]);
    R[9] = 2.0f         * (_v[1] * _v[2] + _v[0] * _v[3]);
    R[10] = 1.0f - 2.0f * (_v[1] * _v[1] + _v[0] * _v[0]);
    R[11] = 0.0f;

    R[12] = 0.0f;
    R[13] = 0.0f;
    R[14] = 0.0f;
    R[15] = 1.0f;
  }

private:

  Vector4 _v;
};

typedef Quaternion<float>  Quaternionf;
typedef Quaternion<double> Quaterniond;

}
}

#endif // __USUL_MATH_QUATERNION_H__
