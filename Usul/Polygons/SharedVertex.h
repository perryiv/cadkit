
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_POLYGONS_SHARED_VERTEX_H__
#define __USUL_POLYGONS_SHARED_VERTEX_H__

#include <list>

namespace Usul {
namespace Polygons {

///////////////////////////////////////////////////////////////////////////////
//
//  Shared Vertex Class
//
///////////////////////////////////////////////////////////////////////////////

template < class Polygon >
class SharedVertex
{
public:
  typedef typename std::list< Polygon *> PolygonList;
  typedef typename PolygonList::iterator Iterator;

  SharedVertex( ) : _polygons(), _visited( false ) { }

  //Append a polygon
  void append( Polygon *p ) { _polygons.push_back( p ); }

  Iterator begin() { return _polygons.begin(); }
  Iterator end()   { return _polygons.end(); }

  bool visited() const  { return _visited; }
  void visited( bool v ) { _visited = v; }

  PolygonList getPolygons() const { return _polygons; }

private:
  PolygonList _polygons;
  bool _visited;
  //TODO Should probably have it's actual Vertex value here too
  //Possibly have the normal so the geometry can be built right from the 
  //Adjacency Map

}; //class SharedVertex


} //namespace Polygons

} //namespace Usul


#endif // __USUL_POLYGONS_SHARED_VERTEX_H__
