
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
//  SlBoundingBox: A template bounding box class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TEMPLATE_BOUNDING_BOX_H_
#define _CADKIT_STANDARD_LIBRARY_TEMPLATE_BOUNDING_BOX_H_

#include "SlMatrix4.h"
#include "SlAssert.h"

// For convenience.
#define SL_BBOX_ZERO ( static_cast<T> ( 0 ) )
#define SL_BBOX_HALF ( static_cast<T> ( 0.5 ) )
#define SL_BBOX_ONE  ( static_cast<T> ( 1 ) )


namespace CadKit
{
template<class T> class SlBoundingBox
{
public:

  typedef T Type; // For the client.
  typedef SlMatrix4<T> Matrix4;
  typedef SlVec3<T> Vec3;
  typedef SlBoundingBox<T> BoundingBox;

  SlBoundingBox();
  SlBoundingBox ( const Vec3 &min, const Vec3 &max );

  bool                        getBoundingSphere ( Vec3 &center, T &radius ) const;
  void                        getCenter ( Vec3 &center ) const;
  Vec3                        getCenter() const;

  const Vec3 &                getMax() const { return _max; }
  Vec3 &                      getMax()       { return _max; }
  const Vec3 &                getMin() const { return _min; }
  Vec3 &                      getMin()       { return _min; }

  T                           getDepth()  const { return _max[2] - _min[2]; }
  T                           getHeight() const { return _max[1] - _min[1]; }
  T                           getWidth()  const { return _max[0] - _min[0]; }
  void                        grow ( const SlBoundingBox &bbox );

  void                        init();
  bool                        intersect ( const Vec3 &linePt, const Vec3 &lineVec, Vec3 &coord );
  const bool &                isValid() const { return _valid; }

  SlBoundingBox &             operator += ( const SlBoundingBox &bbox );

  // Friend function operators. See http://gcc.gnu.org/faq.html#friend 
  // and http://www.bero.org/gcc296.html
#if __GNUC__ >= 2
  template<class P> friend SlBoundingBox<P> operator * ( const SlMatrix4<P> &M, const SlBoundingBox<P> &bbox );
  template<class P> friend SlBoundingBox<P> operator + ( const SlBoundingBox<P> &bbox1, const SlBoundingBox<P> &bbox2 );
#else
  friend SlBoundingBox        operator * ( const Matrix4 &M, const SlBoundingBox &bbox );
  friend SlBoundingBox        operator + ( const SlBoundingBox &bbox1, const SlBoundingBox &bbox2 );
#endif

  void                        setValue ( const Vec3 &min, const Vec3 &max );
  void                        setValue ( const SlBoundingBox &bbox );

  void                        transform ( const Matrix4 &M );

protected:

  SlVec3<T> _min;
  SlVec3<T> _max;
  bool _valid;

  static void                 _grow ( const SlBoundingBox &bbox1, const SlBoundingBox &bbox2, SlBoundingBox &sum );

  static void                 _multiply ( const Matrix4 &M, const SlBoundingBox &bbox, SlBoundingBox &result );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlBoundingBox<T>::SlBoundingBox() :
  _min ( 0, 0, 0 ),
  _max ( 0, 0, 0 ),
  _valid ( false ) 
{
  // Invalid bounding box.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlBoundingBox<T>::SlBoundingBox ( const Vec3 &min, const Vec3 &max ) : 
  _min ( min ),
  _max ( max ),
  _valid ( false ) 
{
  // See if it's _valid.
  _valid = ( ( _min != _max ) && ( _max[0] >= _min[0] ) && ( _max[1] >= _min[1] ) && ( _max[2] >= _min[2] ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlBoundingBox<T>::init()
{
  _min.setValue ( 0, 0, 0 );
  _max.setValue ( 0, 0, 0 );
  _valid = false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlBoundingBox<T>::intersect ( const Vec3 &linePt, const Vec3 &lineVec, Vec3 &coord )
{
  // Do not intersect with an invalid bounding box.
  if ( false == _valid )
    return false;

  // Needed below.
  bool inside = true;
  SlVec3i quadrant;
  Vec3 maxT, candidatePlane;
  const int RIGHT = 0, LEFT = 1, MIDDLE = 2;

  // Find candidate planes.
  if      ( linePt[0] < _min[0] ) { quadrant[0] = LEFT;  candidatePlane[0] = _min[0]; inside = false; }
  else if ( linePt[0] > _max[0] ) { quadrant[0] = RIGHT; candidatePlane[0] = _max[0]; inside = false; }
  else                            { quadrant[0] = MIDDLE; }
  if      ( linePt[1] < _min[1] ) { quadrant[1] = LEFT;  candidatePlane[1] = _min[1]; inside = false; }
  else if ( linePt[1] > _max[1] ) { quadrant[1] = RIGHT; candidatePlane[1] = _max[1]; inside = false; }
  else                            { quadrant[1] = MIDDLE; }
  if      ( linePt[2] < _min[2] ) { quadrant[2] = LEFT;  candidatePlane[2] = _min[2]; inside = false; }
  else if ( linePt[2] > _max[2] ) { quadrant[2] = RIGHT; candidatePlane[2] = _max[2]; inside = false; }
  else                            { quadrant[2] = MIDDLE; }

  // See if ray origin is inside bounding box.
  if ( inside )  
  {
    coord = linePt;
    return true;
  }

  // Calculate T distances to candidate planes.
  maxT[0] = ( MIDDLE != quadrant[0] && 0 != lineVec[0] ) ? ( candidatePlane[0] - linePt[0] ) / lineVec[0] : -1;
  maxT[1] = ( MIDDLE != quadrant[1] && 0 != lineVec[1] ) ? ( candidatePlane[1] - linePt[1] ) / lineVec[1] : -1;
  maxT[2] = ( MIDDLE != quadrant[2] && 0 != lineVec[2] ) ? ( candidatePlane[2] - linePt[2] ) / lineVec[2] : -1;

  // Get largest of the maxT's for final choice of intersection.
  int whichPlane = 0;
  if ( maxT[whichPlane] < maxT[1] ) whichPlane = 1;
  if ( maxT[whichPlane] < maxT[2] ) whichPlane = 2;

  //
  // See if final candidate is actually inside the box.
  //

  if ( maxT[whichPlane] < 0 ) 
    return false;

  if ( 0 != whichPlane ) 
  {
    coord[0] = linePt[0] + maxT[whichPlane] * lineVec[0];
    if ( coord[0] < _min[0] || coord[0] > _max[0] ) 
      return false;
  }
  
  else coord[0] = candidatePlane[0];

  if ( 1 != whichPlane ) 
  {
    coord[1] = linePt[1] + maxT[whichPlane] * lineVec[1];
    if ( coord[1] < _min[1] || coord[1] > _max[1] ) 
      return false;
  }
  
  else coord[1] = candidatePlane[1];

  if ( 2 != whichPlane ) 
  {
    coord[2] = linePt[2] + maxT[whichPlane] * lineVec[2];
    if ( coord[2] < _min[2] || coord[2] > _max[2] ) 
      return false;
  }
  
  else coord[2] = candidatePlane[2];

  return true; // Ray hits box.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlBoundingBox<T>::getBoundingSphere ( Vec3 &center, T &radius ) const
{
  // An invalid bounding box does not have a bounding sphere.
  if ( false == this->isValid() )
    return false;

  // Calculate the bounding sphere.
  center = ( _min + _max ) * SL_BBOX_HALF;
  radius = _min.getDistance ( center );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the center.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlBoundingBox<T>::getCenter ( Vec3 &center ) const 
{
  center.bisect ( _min, _max );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the center.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> SlBoundingBox<T>::getCenter() const 
{
  Vec3 center;
  this->getCenter ( center );
  return center;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the value of the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlBoundingBox<T>::setValue ( const Vec3 &min, const Vec3 &max ) 
{
  _min = min;
  _max = max;

  // See if it's _valid.
  _valid = ( ( _min != _max ) && ( _max[0] >= _min[0] ) && ( _max[1] >= _min[1] ) && ( _max[2] >= _min[2] ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value of the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlBoundingBox<T>::setValue ( const BoundingBox &bbox ) 
{
  _max = bbox._max;
  _min = bbox._min;
  _valid = bbox._valid;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Expands this bounding box by the given one.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlBoundingBox<T>::grow ( const BoundingBox &bbox ) 
{
  // If the given one is invalid then do nothing.
  if ( false == bbox._valid )
  {
    SL_ASSERT ( 0 );
    return;
  }

  // If we are invalid...
  if ( !_valid )
  {
    // Then just set it equal.
    _max = bbox._max;
    _min = bbox._min;

    // Now we are _valid.
    _valid = true;
  }

  // Otherwise...
  else
  {
    // Grow to include the given box.
    this->_grow ( *this, bbox, *this );

    // Now we are _valid.
    _valid = true;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a bounding box that contains both.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlBoundingBox<T>::_grow ( const BoundingBox &b1, const BoundingBox &b2, BoundingBox &sum )
{
  // Only add _valid bounding boxes.
  SL_ASSERT ( b1._valid && b2._valid );

  sum._max[0] = SL_MAX ( b1._max[0], b2._max[0] );
  sum._max[1] = SL_MAX ( b1._max[1], b2._max[1] );
  sum._max[2] = SL_MAX ( b1._max[2], b2._max[2] );

  sum._min[0] = SL_MIN ( b1._min[0], b2._min[0] );
  sum._min[1] = SL_MIN ( b1._min[1], b2._min[1] );
  sum._min[2] = SL_MIN ( b1._min[2], b2._min[2] );

  // Don't assume the sum is valid.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Multiply by the matrix.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlBoundingBox<T>::_multiply ( const Matrix4 &M, const BoundingBox &b1, BoundingBox &b2 )
{
  // Have to make all eight vertives.
  SlVec3<T> v[8];
  v[0].setValue ( b1._min[0], b1._min[1], b1._min[2] ); // Bottom left back.
  v[1].setValue ( b1._min[0], b1._min[1], b1._max[2] ); // Bottom left front.
  v[2].setValue ( b1._max[0], b1._min[1], b1._min[2] ); // Bottom right back.
  v[3].setValue ( b1._max[0], b1._min[1], b1._max[2] ); // Bottom right front.
  v[4].setValue ( b1._min[0], b1._max[1], b1._min[2] ); // Top left back.
  v[5].setValue ( b1._min[0], b1._max[1], b1._max[2] ); // Top left front.
  v[6].setValue ( b1._max[0], b1._max[1], b1._min[2] ); // Top right back.
  v[7].setValue ( b1._max[0], b1._max[1], b1._max[2] ); // Top right front.

  // Pipe all eight vertices through the transformation.
  v[0] = M * v[0];
  v[1] = M * v[1];
  v[2] = M * v[2];
  v[3] = M * v[3];
  v[4] = M * v[4];
  v[5] = M * v[5];
  v[6] = M * v[6];
  v[7] = M * v[7];

  // Pick the max and min coordinates.
  b2._min[0] = SL_MIN ( v[0][0], SL_MIN ( v[1][0], SL_MIN ( v[2][0], SL_MIN ( v[3][0], SL_MIN ( v[4][0], SL_MIN ( v[5][0], SL_MIN ( v[6][0], v[7][0] ) ) ) ) ) ) );
  b2._min[1] = SL_MIN ( v[0][1], SL_MIN ( v[1][1], SL_MIN ( v[2][1], SL_MIN ( v[3][1], SL_MIN ( v[4][1], SL_MIN ( v[5][1], SL_MIN ( v[6][1], v[7][1] ) ) ) ) ) ) );
  b2._min[2] = SL_MIN ( v[0][2], SL_MIN ( v[1][2], SL_MIN ( v[2][2], SL_MIN ( v[3][2], SL_MIN ( v[4][2], SL_MIN ( v[5][2], SL_MIN ( v[6][2], v[7][2] ) ) ) ) ) ) );
  b2._max[0] = SL_MAX ( v[0][0], SL_MAX ( v[1][0], SL_MAX ( v[2][0], SL_MAX ( v[3][0], SL_MAX ( v[4][0], SL_MAX ( v[5][0], SL_MAX ( v[6][0], v[7][0] ) ) ) ) ) ) );
  b2._max[1] = SL_MAX ( v[0][1], SL_MAX ( v[1][1], SL_MAX ( v[2][1], SL_MAX ( v[3][1], SL_MAX ( v[4][1], SL_MAX ( v[5][1], SL_MAX ( v[6][1], v[7][1] ) ) ) ) ) ) );
  b2._max[2] = SL_MAX ( v[0][2], SL_MAX ( v[1][2], SL_MAX ( v[2][2], SL_MAX ( v[3][2], SL_MAX ( v[4][2], SL_MAX ( v[5][2], SL_MAX ( v[6][2], v[7][2] ) ) ) ) ) ) );

  // You have to set this so that you can do this: bbox2 = M * bbox1;
  b2._valid = b1._valid;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Transform the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlBoundingBox<T>::transform ( const Matrix4 &M )
{
  BoundingBox result;
  BoundingBox::_multiply ( M, *this, result );
  this->setValue ( result );
}


// See note: a6b0ab10-8843-11d3-9844-0040054c86c7.


///////////////////////////////////////////////////////////////////////////////
//
//  Expands the bounding box on the left by the one on the right.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlBoundingBox<T> &SlBoundingBox<T>::operator += ( const BoundingBox &bbox ) 
{
  this->grow ( bbox );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  So you can do this: SlBoundingBox bbox2 = M * bbox1.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlBoundingBox<T> operator * ( const SlMatrix4<T> &M, const SlBoundingBox<T> &bbox ) 
{
  SlBoundingBox<T> result;
  SlBoundingBox<T>::_multiply ( M, bbox, result );
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  So you can do this: SlBoundingBox bbox3 = bbox1 + bbox2.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlBoundingBox<T> operator + ( const SlBoundingBox<T> &bbox1, const SlBoundingBox<T> &bbox2 ) 
{
  SlBoundingBox<T> result;
  SlBoundingBox<T>::_grow ( bbox1, bbox2, result );
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Common types.
//
///////////////////////////////////////////////////////////////////////////////

typedef SlBoundingBox<long double> SlBoundingBoxld;
typedef SlBoundingBox<double>      SlBoundingBoxd;
typedef SlBoundingBox<float>       SlBoundingBoxf;


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_TEMPLATE_BOUNDING_BOX_H_
