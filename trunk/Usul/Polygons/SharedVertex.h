
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_POLYGONS_SHARED_VERTEX_H__
#define __USUL_POLYGONS_SHARED_VERTEX_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <list>


namespace Usul {
namespace Polygons {


///////////////////////////////////////////////////////////////////////////////
//
//  Shared Vertex Class
//
///////////////////////////////////////////////////////////////////////////////
template < class Polygon, class ValueType_ >
class SharedVertex : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced BaseClass;

  typedef typename std::list < Polygon * > PolygonList;
  typedef typename PolygonList::iterator Iterator;
  typedef typename ValueType_ ValueType;

  USUL_DECLARE_REF_POINTERS( SharedVertex );

  typedef typename SharedVertex< Polygon, ValueType >::RefPtr Ptr;

  SharedVertex ( const ValueType &v ) : _polygons(), _value ( v ), _visited ( false )
  {
  }

  // Append a polygon
  void append ( Polygon *p )
  {
    _polygons.push_back ( p );
  }

  Iterator begin()
  {
    return _polygons.begin();
  }

  Iterator end()
  {
    return _polygons.end();
  }

  bool visited() const
  {
    return _visited;
  }

  void visited ( bool v )
  {
    _visited = v;
  }

  const PolygonList &polygons() const
  {
    return _polygons;
  }

  const ValueType &value() const
  {
    return _value;
  }

  void value ( const ValueType &v )
  {
    _value = v;
  }

  ValueType normal() const
  {
    ValueType n ( 0, 0, 0 );
    for ( PolygonList::const_iterator i = _polygons.begin(); i != _polygons.end(); ++i )
      n += (*i)->normal();
    return n;
  }

protected:
  SharedVertex( ) : _polygons(), _value(), _visited( false ) { }

  //Use Ref Counting
  ~SharedVertex() {} 

  // No copying.
  SharedVertex ( const SharedVertex & );
  SharedVertex &operator = ( const SharedVertex & );

private:
  PolygonList _polygons;
  ValueType _value;
  bool _visited;
  //Possibly have the normal so the geometry can be built right from the Adjacency Map

}; //class SharedVertex



} // namespace Polygons
} // namespace Usul


#endif // __USUL_POLYGONS_SHARED_VERTEX_H__
