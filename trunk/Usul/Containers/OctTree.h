
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_CONTAINERS_OCT_TREE_CLASS_H_
#define _USUL_CONTAINERS_OCT_TREE_CLASS_H_

#include "Usul/Math/Vector3.h"

#include <vector>


namespace Usul {
namespace Containers {


///////////////////////////////////////////////////////////////////////////////
//
//  Oct-tree class.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class Tester_,
  class ObjectContainer_, 
  class Real_ = double,
  class Vec3_ = Usul::Math::Vector3<RealType_>
>
class OctTree
{
public:

  typedef ObjectContainer_ Objects;
  typedef typename Objects::value_type Object;
  typedef Tester_ Tester;
  typedef std::vector<OctTree> Children;
  typedef typename Children::iterator ChildItr;
  typedef Real_ Real;
  typedef Vec3_ Vec3;
  typedef OctTree<Tester,Objects,Real,Vec3> ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructors.
  //
  /////////////////////////////////////////////////////////////////////////////

  explicit OctTree() : _size ( 0 ), _children(), _min(), _objects(), _tester()
  {
  }
  OctTree ( const OctTree &t ) : _size ( t._size ), _children ( t._children ), _min ( t._min ), _objects ( t._objects ), _tester ( t._tester )
  {
  }
  OctTree ( Real size, Real minX, Real minY, Real minZ ) : _size ( size ), _min ( minX, minY, minZ ), _objects(), _tester()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the size of the cube.
  //
  /////////////////////////////////////////////////////////////////////////////

  Real size() const
  {
    return _size;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Subdivide the given number of times.
  //
  /////////////////////////////////////////////////////////////////////////////

  void subdivide ( unsigned int num )
  {
    if ( 0 == num )
      return;

    _children.reserve ( 8 );
    Real size ( this->size() / 2 );
    _children.push_back ( ThisType ( size, _min[0],        _min[1],        _min[2]        ) );
    _children.push_back ( ThisType ( size, _min[0] + size, _min[1],        _min[2]        ) );
    _children.push_back ( ThisType ( size, _min[0],        _min[1] + size, _min[2]        ) );
    _children.push_back ( ThisType ( size, _min[0] + size, _min[1] + size, _min[2]        ) );
    _children.push_back ( ThisType ( size, _min[0],        _min[1],        _min[2] + size ) );
    _children.push_back ( ThisType ( size, _min[0] + size, _min[1],        _min[2] + size ) );
    _children.push_back ( ThisType ( size, _min[0],        _min[1] + size, _min[2] + size ) );
    _children.push_back ( ThisType ( size, _min[0] + size, _min[1] + size, _min[2] + size ) );

    for ( ChildItr i = _children.begin(); i != _children.end(); ++i )
      i->subdivide ( num - 1 );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if the object is inside of this cube.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool inside ( const Object &object ) const
  {
    _tester ( this->minimum(), this->maximum(), object );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if we can add the object to this cube.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool add ( const Object &object )
  {
    // Is the object in this cube?
    if ( !this->inside ( object ) )
      _objects.push_back ( object );

    // Test the children.
    for ( ChildItr i = _children.begin(); i != _children.end(); ++i )
    {
      // Try to add it to this child.
      if ( i->add ( object ) )
        return true;
    }

    // If we get to here then we could not add the object.
    return false;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the volume.
  //
  /////////////////////////////////////////////////////////////////////////////

  Real volume() const
  {
    return ( _size * _size * _size );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the minimum.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Vec3 &minimum() const
  {
    return _min;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the maximum.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vec3 maximum() const
  {
    Vec3 maximum ( _min[0] + _size, _min[1] + _size, _min[2] + _size );
    return maximum;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the tester.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Tester &tester() const
  {
    return _tester;
  }
  Tester &tester()
  {
    return _tester;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the objects.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Objects &objects() const
  {
    return _objects;
  }
  Objects &objects()
  {
    return _objects;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the children.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Children &children() const
  {
    return _children;
  }
  Children &children()
  {
    return _children;
  }

private:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Private data members.
  //
  /////////////////////////////////////////////////////////////////////////////

  Real _size;
  Children _children;
  Vec3 _min;
  Objects _objects;
  Tester _tester;
};


}; // namespace Containers
}; // namespace Usul


#endif // _USUL_CONTAINERS_OCT_TREE_CLASS_H_
