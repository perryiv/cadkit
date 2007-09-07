
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
//  Policy functor to test if a triangle is in the cube.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class VertexContainer > struct TriangleInside
  {
    TriangleInside() : _v ( 0x0 )
    {
    }
    template < class Vec3, class TriangleType > bool operator () ( const Vec3 &mn, const Vec3 &mx, const TriangleType &t ) const
    {
      const VertexContainer &v = *_v;

      // Determine place of first vertex (in or out).
      bool x0 ( v[t.v0][0] > mn[0] && v[t.v0][0] <= mx[0] );
      bool y0 ( v[t.v0][1] > mn[1] && v[t.v0][1] <= mx[1] );
      bool z0 ( v[t.v0][2] > mn[2] && v[t.v0][2] <= mx[2] );

      // Determine place of second vertex (in or out).
      bool x1 ( v[t.v1][0] > mn[0] && v[t.v1][0] <= mx[0] );
      bool y1 ( v[t.v1][1] > mn[1] && v[t.v1][1] <= mx[1] );
      bool z1 ( v[t.v1][2] > mn[2] && v[t.v1][2] <= mx[2] );

      // Determine place of third vertex (in or out).
      bool x2 ( v[t.v2][0] > mn[0] && v[t.v2][0] <= mx[0] );
      bool y2 ( v[t.v2][1] > mn[1] && v[t.v2][1] <= mx[1] );
      bool z2 ( v[t.v2][2] > mn[2] && v[t.v2][2] <= mx[2] );

      // A point is in if all three of its coordinates are in.
      bool p0 ( x0 && y0 && z0 );
      bool p1 ( x1 && y1 && z1 );
      bool p2 ( x2 && y2 && z2 );

      // A triangle is in if at least two points are in.
      return ( ( p0 && p1 ) || ( p0 && p2 ) || ( p1 && p2 ) );
    }
    const VertexContainer *vertices() const
    {
      return _v;
    }
    const VertexContainer *vertices()
    {
      return _v;
    }
    void vertices ( const VertexContainer *v )
    {
      _v = v;
    }
  private:
    const VertexContainer *_v;
  };
};


///////////////////////////////////////////////////////////////////////////////
//
//  Policy functor to make children adjacent.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  class AdjacentChildren
  {
    template < class OctTreeType, class Real > 
    void _add ( OctTreeType &tree, Real size, const Real &minX, const Real &minY, const Real &minZ ) const
    {
      typedef typename OctTreeType::Children Children;

      // Add the child.
      Children &children = tree.children();
      children.push_back ( new OctTreeType ( size, minX, minY, minZ, &tree ) );
      children.back()->tester().vertices ( tree.tester().vertices() );
    }
  public:
    template < class OctTreeType > void operator () ( OctTreeType &tree ) const
    {
      typedef typename OctTreeType::Real Real;
      typedef typename OctTreeType::Children Children;
      typedef typename Children::iterator ChildItr;
      typedef typename OctTreeType::Vec3 Vec3;

      // Determine new size.
      const Real s ( tree.size() * 0.5f );
      const Vec3 mn ( tree.minimum() );
      const Vec3 mx ( tree.maximum() );

      // Clear and reserve because this works with a list as well as a vector.
      // TODO: add error-checking for existing children?
      Children &children = tree.children();
      children.clear();
      children.reserve ( 8 );

      // Add the children.
      this->_add ( tree, s, mn[0],     mn[1],     mn[2]     );
      this->_add ( tree, s, mn[0] + s, mn[1],     mn[2]     );
      this->_add ( tree, s, mn[0],     mn[1] + s, mn[2]     );
      this->_add ( tree, s, mn[0] + s, mn[1] + s, mn[2]     );
      this->_add ( tree, s, mn[0],     mn[1],     mn[2] + s );
      this->_add ( tree, s, mn[0] + s, mn[1],     mn[2] + s );
      this->_add ( tree, s, mn[0],     mn[1] + s, mn[2] + s );
      this->_add ( tree, s, mn[0] + s, mn[1] + s, mn[2] + s );
    }
  };
};


/////////////////////////////////////////////////////////////////////////////
//
//  Policy class to add an object.
//
/////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct AddIfInside
  {
    template < class OctTreeType, class ObjectType > bool operator () ( OctTreeType &tree, ObjectType &object ) const
    {
      typedef typename OctTreeType::Children Children;
      typedef typename OctTreeType::Objects Objects;
      typedef typename Children::iterator ChildItr;
      typedef typename ObjectType::CubeType CubeType;

      // Is the object in this cube?
      if ( !tree.inside ( object ) )
        return false;

      // Shortcuts.
      Children &children = tree.children();
      Objects &objects   = tree.objects();

      // Add the object to this cube.
      object.cube = reinterpret_cast < CubeType * > ( &tree );
      objects.push_back ( object );

      // Make sure there are children.
      if ( children.empty() )
        tree.subdivide ( 1 );

      // Test the children.
      for ( ChildItr i = children.begin(); i != children.end(); ++i )
      {
        // Try to add it to this child.
        if ( (*i)->add ( object ) )
          break;
      }

      // We at least added the object to the given cube.
      return true;
    }
  };
};


///////////////////////////////////////////////////////////////////////////////
//
//  Triangle class.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct Triangle
  {
    typedef void * CubeType;
    typedef unsigned int IndexType;
    Triangle() : v0(0), v1(0), v2(0), cube(0){}
    Triangle ( IndexType i0, IndexType i1, IndexType i2 ) : v0(i0), v1(i1), v2(i2), cube(0) {}
    IndexType v0;
    IndexType v1;
    IndexType v2;
    CubeType cube;

    bool operator==( const Triangle& tri )
    {
      return (this->v0 == tri.v0 && this->v1 == tri.v1 && this->v2 == tri.v2 );
    }
  };
};


///////////////////////////////////////////////////////////////////////////////
//
//  Oct-tree class.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ObjectContainer_, 
  class ChildMaker_   = Usul::Containers::Detail::AdjacentChildren,
  class Tester_       = Usul::Containers::Detail::TriangleInside,
  class Adder_        = Usul::Containers::Detail::AddIfInside,
  class Real_         = double,
  class Vec3_         = Usul::Math::Vector3<Real_>
>
class OctTree
{
public:

  typedef ObjectContainer_ Objects;
  typedef typename Objects::value_type Object;
  typedef Tester_ Tester;
  typedef Adder_ Adder;
  typedef ChildMaker_ ChildMaker;
  typedef std::vector<OctTree*> Children;
  typedef typename Children::iterator ChildItr;
  typedef Real_ Real;
  typedef Vec3_ Vec3;
  typedef OctTree<Objects,ChildMaker,Tester,Real,Vec3> ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructors.
  //
  /////////////////////////////////////////////////////////////////////////////

  explicit OctTree ( OctTree *p = 0x0 ) : _size ( 0 ), _children(), _min(), _objects(), _tester(), _childMaker(), _adder(), _parent ( p )
  {
  }
  OctTree ( const OctTree &t ) : _size ( t._size ), _children ( t._children ), _min ( t._min ), _objects ( t._objects ), _tester ( t._tester ), _childMaker ( t._childMaker ), _adder ( t._adder ), _parent ( t._parent )
  {
  }
  OctTree ( Real size, Real minX, Real minY, Real minZ, OctTree *p = 0x0 ) : _size ( size ), _min ( minX, minY, minZ ), _objects(), _tester(), _childMaker(), _adder(), _parent ( p )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ~OctTree()
  {
    for ( ChildItr i = _children.begin(); i != _children.end(); ++i )
      delete (*i);
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
    // Return now if we are at the end.
    if ( 0 == num )
      return;

    // Make the children.
    _childMaker ( *this );

    // Divide the children.
    for ( ChildItr i = _children.begin(); i != _children.end(); ++i )
      (*i)->subdivide ( num - 1 );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if the object is inside of this cube.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool inside ( const Object &object ) const
  {
    return _tester ( this->minimum(), this->maximum(), object );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if we can add the object to this cube.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool add ( Object &object )
  {
    return _adder ( *this, object );
  }

  void remove ( Object &object )
  {
    std::remove( _objects.begin(), _objects.end(), object);
    for( ChildItr i = _children.begin(); i != _children.end(); ++i )
       (*i)->remove( object );
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


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the child-maker.
  //
  /////////////////////////////////////////////////////////////////////////////

  const ChildMaker &childMaker() const
  {
    return _childMaker;
  }
  ChildMaker &childMaker()
  {
    return _childMaker;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the adder.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Adder &adder() const
  {
    return _adder;
  }
  Adder &adder()
  {
    return _adder;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set/get the parent.
  //
  /////////////////////////////////////////////////////////////////////////////

  const OctTree *parent() const
  {
    return _parent;
  }
  OctTree *parent()
  {
    return _parent;
  }
  void parent ( OctTree *p )
  {
    _parent = p;
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
  ChildMaker _childMaker;
  Adder _adder;
  OctTree *_parent;
};


} // namespace Containers
} // namespace Usul


#endif // _USUL_CONTAINERS_OCT_TREE_CLASS_H_
