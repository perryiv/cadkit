
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A bounding box class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_BOUNDING_BOX_H_
#define _GENERIC_SCENE_GRAPH_CORE_BOUNDING_BOX_H_

#include "GSG/Core/Math.h"


namespace GSG {
namespace Detail {


template < class Vector_ > class BoundingBox
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef Vector_ Vector;
  typedef typename Vector::DataType DataType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  explicit BoundingBox() :
    _min ( 0, 0, 0 ),
    _max ( 0, 0, 0 )
  {
    // Empty.
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  BoundingBox ( const BoundingBox &bb ) :
    _min ( bb._min ),
    _max ( bb._max )
  {
    // Empty.
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  BoundingBox ( const Vector &min, const Vector &max ) :
    _min ( min ),
    _max ( max )
  {
    // Empty.
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if it's valid.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool valid() const
  {
    return
      ( _min != _max ) && 
      ( _max[0] >= _min[0] ) && 
      ( _max[1] >= _min[1] ) && 
      ( _max[2] >= _min[2] );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Invalidate.
  //
  /////////////////////////////////////////////////////////////////////////////

  void invalidate()
  {
    _min = _max;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the bounding sphere.
  //
  /////////////////////////////////////////////////////////////////////////////

  void getBoundingSphere ( Vector &center, DataType &radius ) const
  {
    // Calculate the bounding sphere.
    this->getCenter ( center );
    radius = GSG::distance ( _min, center );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the bounding sphere.
  //
  /////////////////////////////////////////////////////////////////////////////

  void getCenter ( Vector &center ) const
  {
    center = ( _min + _max ) * static_cast < DataType > ( 0.5 );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access to the bounds.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Vector &              max() const { return _max; }
  Vector &                    max()       { return _max; }
  const Vector &              min() const { return _min; }
  Vector &                    min()       { return _min; }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the dimensions.
  //
  /////////////////////////////////////////////////////////////////////////////

  DataType                    depth()  const { return _max[2] - _min[2]; }
  DataType                    height() const { return _max[1] - _min[1]; }
  DataType                    width()  const { return _max[0] - _min[0]; }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Grow to accommodate the given bounding box.
  //
  /////////////////////////////////////////////////////////////////////////////

  void grow ( const BoundingBox &bb )
  {
    // If the given one is invalid then do nothing.
    if ( false == bbox.valid() )
      return;

    // If we are invalid...
    if ( false == this->valid() )
    {
      // Then just set it equal.
      _max = bbox._max;
      _min = bbox._min;

      // We should be valid.
      _valid = true;
    }

    // Otherwise...
    else
    {
      // Grow to include the given box.
      _min[0] = GSG::minimum ( bb._min[0], _min[0] );
      _min[1] = GSG::minimum ( bb._min[1], _min[1] );
      _min[2] = GSG::minimum ( bb._min[2], _min[2] );
      _max[0] = GSG::maximum ( bb._max[0], _max[0] );
      _max[1] = GSG::maximum ( bb._max[1], _max[1] );
      _max[2] = GSG::maximum ( bb._max[2], _max[2] );

      // If we grow to include a valid bounding box, then we should be valid.
      _valid = true;
    }

    // Sanity check.
    GSG_ASSERT ( this->valid() );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Grow to accommodate the given bounding box.
  //
  /////////////////////////////////////////////////////////////////////////////

  BoundingBox &operator += ( const BoundingBox &bb )
  {
    this->grow();
    return *this;
  }


private:

  Vector _min;
  Vector _max;
  mutable bool _valid;
};


}; // namespace Detail
}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_BOUNDING_BOX_H_
