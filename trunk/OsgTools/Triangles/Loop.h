
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __EDIT_POLYGONS_LOOP_H__
#define __EDIT_POLYGONS_LOOP_H__

#include "OsgTools/Export.h"
#include "OsgTools/Triangles/SharedVertex.h"

#include "Usul/Math/Vector3.h"

#include "Usul/Interfaces/IUnknown.h"

#include "osg/Vec3"
#include "osg/Array"
#include "osg/Quat"
#include "osg/ref_ptr"
#include "osg/Geometry"

#include <list>

namespace OsgTools {
namespace Triangles {

class OSG_TOOLS_EXPORT Loop
{
public:
  typedef OsgTools::Triangles::SharedVertex::ValidRefPtr SharedVertexPtr;
  typedef SharedVertexPtr value_type;
  typedef std::vector< value_type > Points;
  typedef Points::iterator iterator;
  typedef Points::const_iterator const_iterator;

  Loop();
  ~Loop();

  void push_back( const value_type& v ) { _loop.push_back( v ); }

  bool triangulate( Usul::Interfaces::IUnknown *caller );

  iterator begin() { return _loop.begin(); }
  iterator end()   { return _loop.end(); }

  const_iterator begin() const { return _loop.begin(); }
  const_iterator end()   const { return _loop.end();   }

  //Is this loop empty?
  bool empty() const { return _loop.empty(); }

  //get the size
  unsigned int size() const { return _loop.size(); }

  //Check to see if the given point is inside this loop
  bool pointInside( const osg::Vec3 &, Usul::Interfaces::IUnknown *caller ) const;

  //Add a loop to the list of inner loops
  void addInnerLoop ( const Loop& loop ) { _innerLoops.push_back( loop._loop ); }

  bool hasInnerLoop ( ) const { return !_innerLoops.empty(); }

  //Build frame data from this loop
  void getFrameData ( osg::Vec3&, float &, osg::Quat&, Usul::Interfaces::IUnknown *caller ) const;

  //Get the number of planes that this loop covers
  unsigned int getNumPlanes() const;

  //Flip the normals that this loop made
  void flipNormals();

  //Undo any changed made
  void undo();

  //Get the geometry
  //osg::Geometry* geometry() { return _geometry.get(); }

  //Get/Set the starting value
  unsigned int start () const               { return _start; }
  void         start ( unsigned int start ) { _start = start; }

  //Get the number of triangles
  unsigned int triangles () const { return _numTriangles; }

  const osg::Vec3f& vertex ( unsigned int i, Usul::Interfaces::IUnknown *caller  ) const;

private:
  typedef std::vector< Usul::Math::Vec3i > Triangles;

  //Build points needed for the algorithm
  void _buildPoints ( std::list< unsigned int > &sizes, osg::Vec2Array &points, Usul::Interfaces::IUnknown *caller  );

  void _triangulate ( Triangles &triangles, Usul::Interfaces::IUnknown *caller );

  Points _loop;

  std::list< Points > _innerLoops;

  //Our starting location in the vertex sequence
  unsigned int _start;

  //The number of triangles that we made
  unsigned int _numTriangles;

};//class Loop

} //namespace Triangles
} //namespace OsgTools


#endif // __EDIT_POLYGONS_LOOP_H__
