
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A bounding sphere class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_BOUNDING_SPHERE_H_
#define _GENERIC_SCENE_GRAPH_CORE_BOUNDING_SPHERE_H_

#include "GSG/Core/Math.h"
#include "GSG/Core/Assert.h"

#include "boost/mpl/assert_is_same.hpp"


namespace GSG {
namespace Detail {


template < class Vector_ > class BoundingSphere
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef Vector_ VectorType;
  typedef typename VectorType::value_type value_type;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  explicit BoundingSphere() :
    _center ( 0, 0, 0 ),
    _radius ( -1 )
  {
    // Empty.
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  BoundingSphere ( const BoundingSphere &bb ) :
    _center ( bb._center ),
    _radius ( bb._radius )
  {
    // Empty.
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  BoundingSphere ( const VectorType &center, value_type radius ) :
    _center ( center ),
    _radius ( radius )
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
    return ( _radius >= 0 );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Invalidate.
  //
  /////////////////////////////////////////////////////////////////////////////

  void invalidate()
  {
    _radius = -1;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the bounding sphere.
  //
  /////////////////////////////////////////////////////////////////////////////

  void get ( VectorType &center, value_type &radius ) const
  {
    center = _center;
    radius = _radius;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the center.
  //
  /////////////////////////////////////////////////////////////////////////////

  VectorType &center()
  {
    return _center;
  }
  const VectorType &center() const
  {
    return _center;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the radius.
  //
  /////////////////////////////////////////////////////////////////////////////

  value_type &radius()
  {
    return _radius;
  }
  const value_type &radius() const
  {
    return _radius;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set this instance.
  //
  /////////////////////////////////////////////////////////////////////////////

  void set ( const BoundingSphere &bs )
  {
    _center = bs._center;
    _radius = bs._radius;
  }
  void set ( const VectorType &c, value_type r )
  {
    _center = c;
    _radius = r;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  BoundingSphere &operator = ( const BoundingSphere &bs )
  {
    this->set ( bs );
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Grow to include the given vector.
  //
  /////////////////////////////////////////////////////////////////////////////

  void grow ( const VectorType &point )
  {
    // Just set this sphere if we are invalid.
    if ( !this->valid() )
    {
      this->set ( point, 0 );
      return;
    }

    // If the point is the center...
    if ( point.equal ( _center ) )
      return;

    // Calculate the distance to the point.
    value_type dist ( ( point - _center ).length() );

    // If the distance is bigger than the existing radius...
    if ( dist > _radius )
      _radius = dist;

    // Sanity check.
    GSG_ASSERT ( this->valid() );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Grow to include the given bounding sphere.
  //
  /////////////////////////////////////////////////////////////////////////////

  void grow ( const BoundingSphere &bs )
  {
    // Ignore bad input.
    if ( !bs.valid() )
      return;

    // Just set this sphere if we are invalid.
    if ( !this->valid() )
    {
      this->set ( bs );
      return;
    }

    // If the centers are identical...
    if ( _center.equal ( bs.center() ) )
    {
      // if the given radius is bigger...
      if ( bs.radius() > _radius )
      {
        _radius = bs.radius();
        return;
      }
    }

    // Calculate the vector between the two centers.
    VectorType c2c ( bs.center() - _center );

    // The length of this vector.
    value_type lc2c ( c2c.length() );

    // Handle numerical problems.
    if ( 0 == lc2c )
      return;

    // If the given sphere is outside of this sphere...
    if ( lc2c + bs.radius() > _radius )
    {
      // For speed.
      value_type ilc2c ( static_cast < value_type > ( 1 ) / lc2c );

      // Calculate intermediate vectors that are offset from the centers.
      VectorType temp1 ( _center - ( c2c * ( _radius * ilc2c ) ) );
      VectorType temp2 ( bs.center() + ( c2c * ( bs.radius() * ilc2c ) ) );

      // Set this sphere.
      VectorType c ( ( temp1 + temp2 ) * static_cast < value_type > ( 0.5 ) );
      value_type r ( ( temp2 - _center ).length() );
      this->set ( c, r );
    }

    // Sanity check.
    GSG_ASSERT ( this->valid() );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Grow to include the given bounding box.
  //
  /////////////////////////////////////////////////////////////////////////////

  void grow ( const VectorType &min, const VectorType &max )
  {
    // Ignore bad input.
    if ( min == max )
      return;

    // Calculate the bounding sphere.
    VectorType center ( ( min + max ) * static_cast < value_type > ( 0.5 ) );
    value_type radius ( GSG::abs ( GSG::distance ( center, max ) ) );

    // Grow to include the bounding sphere.
    this->grow ( BoundingSphere ( center, radius ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Grow to include the given bounding box.
  //
  /////////////////////////////////////////////////////////////////////////////

  BoundingSphere &operator += ( const BoundingSphere &bs )
  {
    this->grow ( bs );
    return *this;
  }

private:

  VectorType _center;
  value_type _radius;
};


/////////////////////////////////////////////////////////////////////////////
//
//  Functor to accumulate the bounding sphere.
//
/////////////////////////////////////////////////////////////////////////////

template < class BoundingSphere_ > struct GetBoundOfObjects
{
  typedef BoundingSphere_ BSphere;

  GetBoundOfObjects() : _bound(){}
  GetBoundOfObjects ( const GetBoundOfObjects &g ) : _bound ( g._bound ){}

  template < class Object_ > void operator () ( Object_ &object )
  {
    object->calculateBoundingSphere();
    _bound.grow ( object->boundingSphere() );
  }

  const BSphere &get() const
  {
    return _bound;
  }

protected:

  BSphere _bound;
};


/////////////////////////////////////////////////////////////////////////////
//
//  Functor to accumulate the bounding sphere.
//
/////////////////////////////////////////////////////////////////////////////

template < class BoundingSphere_, class Pool_ > struct GetBoundOfPoints
{
  typedef BoundingSphere_ BSphere;
  typedef Pool_ Pool;
  typedef typename Pool::value_type value_type;
  typedef typename Pool::size_type size_type;
  typedef typename Pool::ValidPtr PoolPtr;
  BOOST_MPL_ASSERT_IS_SAME ( value_type, BOOST_DEDUCED_TYPENAME BSphere::VectorType );

  GetBoundOfPoints ( Pool *pool ) : _bound(), _pool ( pool ){}
  GetBoundOfPoints ( const GetBoundOfPoints &g ) : _bound ( g._bound ), _pool ( g._pool ){}

  void operator () ( size_type i ) const
  {
    _bound.grow ( _pool->value ( i ) );
  }

  void operator () ( size_type i )
  {
    _bound.grow ( _pool->value ( i ) );
  }

  const BSphere &get() const
  {
    return _bound;
  }

protected:

  BSphere _bound;
  PoolPtr _pool;
};


}; // namespace Detail
}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_BOUNDING_SPHERE_H_
