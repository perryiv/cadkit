
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
//  SlLine3: A 3D line.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TEMPLATE_3D_LINE_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_TEMPLATE_3D_LINE_CLASS_H_

#include "SlVec3.h"
#include "SlGeometry.h"

#ifdef CADKIT_DEFINE_SL_LINE_3_MATRIX_FUNCTIONS
# include "SlMatrix4.h"
#endif


namespace CadKit
{
template<class T> class SlLine3
{
public:

  typedef T Real; // For readability.
  typedef T Type; // For the client.
  typedef SlLine3<T> Line3;
  typedef SlVec3<T> Vec3;

  SlLine3();
  SlLine3 ( const Vec3 &pt, const Vec3 &vec );

  bool            getClosestPoint ( const Line3 &line, Vec3 &answer ) const;
  Vec3            getClosestPoint ( const Vec3 &pt, Real &t ) const;
  Vec3            getClosestPoint ( const Vec3 &pt ) const;
  Real            getDistance ( const Vec3 &pt ) const;
  Real            getDistanceSquared ( const Vec3 &pt ) const;
  const Vec3 &    getDirection() const { return _vec; }
  Vec3 &          getDirection()       { return _vec; }
  void            getDirection ( Vec3 &vec ) { vec = _vec; }
  const Vec3 &    getPoint() const { return _pt; }
  Vec3 &          getPoint()       { return _pt; }
  void            getPoint ( Vec3 &pt ) { pt = _pt; }
  void            getValue ( Vec3 &pt, Vec3 &vec ) const { pt = _pt; vec = _vec; }

  bool            isEqualTo ( const Line3 &line ) const;
  bool            isEqualTo ( const Line3 &line, const T &tolerance ) const;
  bool            isNotEqualTo ( const Line3 &line ) const;
  bool            isNotEqualTo ( const Line3 &line, const T &tolerance ) const;
  bool            isOnLine ( const Vec3 &pt ) const;
  bool            isParallelTo ( const Line3 &line ) const;
  bool            isPerpendicularTo ( const Line3 &line ) const;

  // Friend function operators. See http://gcc.gnu.org/faq.html#friend 
  // and http://www.bero.org/gcc296.html
#if __GNUC__ >= 2
  template<class P> friend bool operator == ( const SlLine3<P> &line1, const SlLine3<P> &line2 );
  template<class P> friend bool operator != ( const SlLine3<P> &line1, const SlLine3<P> &line2 );
#else
  friend bool     operator == ( const Line3 &line1, const Line3 &line2 );
  friend bool     operator != ( const Line3 &line1, const Line3 &line2 );
#endif

  void            normalize() { _vec.normalize(); }

  void            setDirection ( const Vec3 &vec ) { _vec = vec; }
  void            setPoint ( const Vec3 &pt ) { _pt = pt; }
  void            setValue ( const Line3 &line );
  void            setValue ( const Vec3 &pt, const Vec3 &vec );

  #ifdef CADKIT_DEFINE_SL_LINE_3_MATRIX_FUNCTIONS
  void            transform ( const SlMatrix4<T> &M );
  #endif

protected:

  Vec3 _pt;  // A point on the line.
  Vec3 _vec; // The direction vector.
};



///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlLine3<T>::SlLine3() :
  _pt  ( 0, 0, 0 ),
  _vec ( 1, 0, 0 )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlLine3<T>::SlLine3 ( const Vec3 &pt, const Vec3 &vec ) :
  _pt  ( pt ),
  _vec ( vec )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the point on this line that's closest to the given line.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlLine3<T>::getClosestPoint ( const Line3 &line, Vec3 &answer ) const
{
  // For readability. This instance is "line1" and the given line is "line2".
  const Vec3 &v1 = _vec;
  const Vec3 &v2 = line._vec;
  const Vec3 &p1 = _pt;
  const Vec3 &p2 = line._pt;

  // If the cross product is zero then the lines are parallel.
  Vec3 v3 = v1.cross ( v2 );
  if ( 0 == v3[0] && 0 == v3[1] && 0 == v3[2] ) 
    return false;

  //
  // Define a plane that contains line2 and is oriented to be parallel with v3.
  // Intersecting that plane with line1 will produce the point that is on line1
  // and is closest to line2.
  //

  // We need to make a normal for our plane.
  Vec3 pn = v2.cross ( v3 );
  pn.normalize();

  // The plane's reference point is the line2's reference point.
  const Vec3 &pp = p2;

  // Intersect the plane and line1.
  return CadKit::intersectPlaneAndLine ( p1, v1, pp, pn, answer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the closest point on this line with the given point.
//  See notes 18 June 1999, page 1.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> SlLine3<T>::getClosestPoint ( const Vec3 &pt, T &t ) const
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

template<class T> inline SlVec3<T> SlLine3<T>::getClosestPoint ( const Vec3 &pt ) const
{
  Real t;
  return this->getClosestPoint ( pt, t );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the squared distance to the line from the given point. This function 
//  assumes that the data member "_vec" has been normalized.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlLine3<T>::getDistanceSquared ( const Vec3 &pt ) const
{
  Vec3 temp ( pt - _pt );
  temp = temp.cross ( _vec );
  return temp.dot ( temp );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the distance to the line from the given point. This function 
//  assumes that the data member "_vec" has been normalized.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlLine3<T>::getDistance ( const Vec3 &pt ) const
{
  return CadKit::getSquareRoot ( this->getDistanceSquared ( pt ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are equal within the given tolerance.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlLine3<T>::isEqualTo ( const SlLine3<T> &line, const T &tolerance ) const
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

template<class T> inline bool SlLine3<T>::isNotEqualTo ( const SlLine3<T> &line, const T &tolerance ) const
{
  return ( false == this->isEqualTo ( vec, tolerance ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlLine3<T>::isEqualTo ( const SlLine3<T> &line ) const
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

template<class T> inline bool SlLine3<T>::isNotEqualTo ( const SlLine3<T> &line ) const
{
  return ( false == this->isEqualTo ( vec ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator == ( const SlLine3<T> &line1, const SlLine3<T> &line2 )
{
  return line1.isEqualTo ( line2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Inequality.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator != ( const SlLine3<T> &line1, const SlLine3<T> &line2 )
{
  return line1.isNotEqualTo ( line2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this point on the line? See notes 18 June 1999, page 1.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlLine3<T>::isOnLine ( const Vec3 &pt ) const
{
  Vec3 temp ( pt - _pt );
  temp = temp.cross ( _vec );
  return ( 0 == temp[0] && 0 == temp[1] && 0 == temp[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given line is parallel to this line.
//  This requires that the direction vectors (_vec) be normalized.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlLine3<T>::isParallelTo ( const Line3 &line ) const
{
  return ( _vec == line._vec || _vec == -(line._vec) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given line is parallel to this line.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlLine3<T>::isPerpendicularTo ( const Line3 &line ) const
{
  return ( 0 == _vec.dot ( line._vec ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlLine3<T>::setValue ( const Vec3 &pt, const Vec3 &vec )
{
  _pt = pt;
  _vec = vec;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlLine3<T>::setValue ( const Line3 &line )
{
  this->setValue ( line._pt, line._vec );
}


#ifdef CADKIT_DEFINE_SL_LINE_3_MATRIX_FUNCTIONS


///////////////////////////////////////////////////////////////////////////////
//
//  Transform the line. This may scale the vector so the client should call
//  normalize() after this (if they want a normalized vector).
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlLine3<T>::transform ( const SlMatrix4<T> &M )
{
  // Transform the point with the given matrix.
  _pt = M * _pt;

  // Get a matrix that doesn't have any translations.
  SlMatrix4<T> RS;
  M.getRotationAndScale ( RS );

  // Transform the vector with the rotation-and-scale matrix (we don't want 
  // to "translate" the direction vector, it doesn't make sense to do so).
  _vec = RS * _vec;
}


#endif // CADKIT_DEFINE_SL_LINE_3_MATRIX_FUNCTIONS


///////////////////////////////////////////////////////////////////////////////
//
//	Common types.
//
///////////////////////////////////////////////////////////////////////////////

typedef SlLine3<long double> SlLine3ld;
typedef SlLine3<double>      SlLine3d;
typedef SlLine3<float>       SlLine3f;


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_TEMPLATE_3D_LINE_CLASS_H_
