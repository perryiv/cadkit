
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlLine2: A 2D line.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TEMPLATE_2D_LINE_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_TEMPLATE_2D_LINE_CLASS_H_

#include "SlVec2.h"

#define SL_LINE2_ONE  ( static_cast<T>(1) )


namespace CadKit
{
template<class T> class SlLine2
{
public:

  typedef T Real; // For readability.
  typedef T Type; // For the client.
  typedef SlLine2<T> Line2;
  typedef SlVec2<T> Vec2;

  SlLine2();
  SlLine2 ( const Vec2 &pt, const Vec2 &vec );

  Vec2            getClosestPoint ( const Vec2 &pt, Real &t ) const;
  Vec2            getClosestPoint ( const Vec2 &pt ) const;
  Real            getDistance ( const Vec2 &pt ) const;
  Real            getDistanceSquared ( const Vec2 &pt ) const;
  void            getValue ( Vec2 &pt, Vec2 &vec ) const { pt = _pt; vec = _vec; }

  bool            isEqualTo ( const SlLine2 &line ) const;
  bool            isEqualTo ( const SlLine2 &line, const T &tolerance ) const;
  bool            isNotEqualTo ( const SlLine2 &line ) const;
  bool            isNotEqualTo ( const SlLine2 &line, const T &tolerance ) const;
  bool            isOnLine ( const Vec2 &pt ) const;
  bool            isParallelTo ( const SlLine2 &line ) const;
  bool            isPerpendicularTo ( const SlLine2 &line ) const;
  bool            intersect ( const SlLine2 &line, Vec2 &answer ) const;
  bool            intersect ( const Vec2 &linePoint, const Vec2 &lineDir, Vec2 &answer ) const;

  // Friend function operators. See http://gcc.gnu.org/faq.html#friend 
  // and http://www.bero.org/gcc296.html
#if __GNUC__ >= 2
  template<class P> friend bool operator == ( const SlLine2<P> &line1, const SlLine2<P> &line2 );
  template<class P> friend bool operator != ( const SlLine2<P> &line1, const SlLine2<P> &line2 );
#else
  friend bool     operator == ( const Line2 &line1, const Line2 &line2 );
  friend bool     operator != ( const Line2 &line1, const Line2 &line2 );
#endif

  void            normalize() { _vec.normalize(); }

  void            setValue ( const SlLine2 &line );
  void            setValue ( const Vec2 &pt, const Vec2 &vec );

protected:

  Vec2 _pt;  // A point on the line.
  Vec2 _vec; // The direction vector.
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlLine2<T>::SlLine2() :
  _pt  ( 0, 0 ),
  _vec ( 1, 0 )
{
  // Empty.
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlLine2<T>::SlLine2 ( const Vec2 &pt, const Vec2 &vec ) :
  _pt  ( pt ),
  _vec ( vec )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the closest point on the line from the given point. The value t 
//  will hold the parametric value on the line where the closest point 
//  is. If the point returned is P, then P = _pt + t * _vec.
//  See notes 18 June 1999, page 1.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T> SlLine2<T>::getClosestPoint ( const Vec2 &pt, T &t ) const
{
  t = ( pt.dot ( _vec ) - _pt.dot ( _vec ) ) / _vec.dot ( _vec );
  return _pt + t * _vec;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the closest point on this line with the given point.
//  See notes 18 June 1999, page 1.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T> SlLine2<T>::getClosestPoint ( const Vec2 &pt ) const
{
  Real t;
  return this->getClosestPoint ( pt, t );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the distance to the line from the given point. This function 
//  assumes that the data member "_vec" has been normalized.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlLine2<T>::getDistance ( const Vec2 &pt ) const
{
  // Can't use cross product like in the 3D line classes. So we 
  // find the closest point on the line to the given point and 
  // then find the distance between them. There is probably a way 
  // that is optimized for 2D (using the implicit representation).
  return pt.getDistance ( this->getClosestPoint ( pt ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the squared distance to the line from the given point. This 
//  function assumes that the data member "_vec" has been normalized.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlLine2<T>::getDistanceSquared ( const Vec2 &pt ) const
{
  // Can't use cross product like in the 3D line classes. So we 
  // find the closest point on the line to the given point and 
  // then find the distance between them. There is probably a way 
  // that is optimized for 2D (using the implicit representation).
  return pt.getDistanceSquared ( this->getClosestPoint ( pt ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are equal within the given tolerance.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlLine2<T>::isEqualTo ( const SlLine2<T> &line, const T &tolerance ) const
{
  return 
    _pt.isEqualTo ( line._pt, tolerance ) && 
    _vec.isEqualTo ( line._vec, tolerance );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are not equal within the given tolerance.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlLine2<T>::isNotEqualTo ( const SlLine2<T> &line, const T &tolerance ) const
{
  return ( false == this->isEqualTo ( vec, tolerance ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlLine2<T>::isEqualTo ( const SlLine2<T> &line ) const
{
  return 
    _pt.isEqualTo ( line._pt ) && 
    _vec.isEqualTo ( line._vec );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are not equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlLine2<T>::isNotEqualTo ( const SlLine2<T> &line ) const
{
  return ( false == this->isEqualTo ( vec ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator == ( const SlLine2<T> &line1, const SlLine2<T> &line2 )
{
  return line1.isEqualTo ( line2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Inequality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator != ( const SlLine2<T> &line1, const SlLine2<T> &line2 )
{
  return line1.isNotEqualTo ( line2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this point on the line? See notes 18 June 1999, page 3.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlLine2<T>::isOnLine ( const Vec2 &pt ) const
{
  Vec2 A ( pt - _pt );
  return ( 0 == CadKit::getArcCosine ( A.getAngle ( _vec ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given line is parallel to this line.
//  This requires that the direction vectors (_vec) be normalized.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlLine2<T>::isParallelTo ( const SlLine2 &line ) const
{
  return ( _vec == line._vec || _vec == -(line._vec) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given line is parallel to this line.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlLine2<T>::isPerpendicularTo ( const SlLine2 &line ) const
{
  return ( 0 == _vec.dot ( line._vec ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the intersection. Return false if they are parallel.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlLine2<T>::intersect ( const Vec2 &linePoint, const Vec2 &lineDir, Vec2 &answer ) const
{
  if ( this->isParallelTo ( line ) ) 
    return false;

  Real dx, dy, A, B, C, x, y, m, a[2][2], b[2], X[2], denom, factor;
  Real aInv[2][2];

  //
  // From the vector and the point we can develop the equation of the
  // line in the form Ax + By = C.  Catch the degenerate case where 
  // dx = 0 (vertical line).
  //

  dx = _vec[0];

  if ( 0 == dx )
  {
    A = 1.0;
    B = 0.0;
    C = _pt[0];
  }

  else
  {
    dy = _vec[1];
    A = -dy;
    B = dx;
    y = _pt[1];
    x = _pt[0];
    m = dy / dx;
    C = ( y - m * x ) * B;
  }

  // Put these coeficients into the first row of the 2x2 matrix.
  a[0][0] = A;
  a[0][1] = B;
  b[0] = C;

  //
  // Do the same for the other point and vector.
  //

  dx = line._vec[0];

  if ( 0 == dx )
  {
    A = 1.0;
    B = 0.0;
    C = line._pt[0];
  }

  else
  {
    dy = line._vec[1];
    A = -dy;
    B = dx;
    y = line._pt[1];
    x = line._pt[0];
    m = dy / dx;
    C = ( y - m * x ) * B;
  }

  // Store these coeficients in the same matrix in the second row.
  a[1][0] = A;
  a[1][1] = B;
  b[1] = C;

  // Have to find the inverse of matrix "a".
  denom = a[0][0] * a[1][1] - a[0][1] * a[1][0];

  // If this matrix is gonna give us problems then this method failed.
  if ( 0 == denom )
    return false;

  // A 2x2 inversion is straight forward.
  factor = SL_LINE2_ONE / denom;
  aInv[0][0] =  factor * a[1][1];
  aInv[0][1] = -factor * a[0][1];
  aInv[1][0] = -factor * a[1][0];
  aInv[1][1] =  factor * a[0][0];

  // Now the solution vector is found from X = aInv * b where aInv is
  // the inverse of the matrix "a".
  answer[0] = aInv[0][0] * b[0] + aInv[0][1] * b[1];
  answer[1] = aInv[1][0] * b[0] + aInv[1][1] * b[1];

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the intersection. Return false if they are parallel.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlLine2<T>::intersect ( const SlLine2 &line, Vec2 &answer ) const
{
  // Call the other one.
  this->intersect ( line.getPoint(), line.getDirection(), answer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlLine2<T>::setValue ( const Vec2 &pt, const Vec2 &vec )
{
  _pt = pt;
  _vec = vec;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlLine2<T>::setValue ( const SlLine2<T> &line )
{
  this->setValue ( line._pt, line._vec );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Common types.
//
///////////////////////////////////////////////////////////////////////////////

typedef SlLine2<long double> SlLine2ld;
typedef SlLine2<double>      SlLine2d;
typedef SlLine2<float>       SlLine2f;


}; // namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_TEMPLATE_2D_LINE_CLASS_H_
