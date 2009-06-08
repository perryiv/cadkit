
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
#include "OsgTools/Configure/OSG.h"
#include "OsgTools/Triangles/SharedVertex.h"

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
  // Typedefs.
  typedef OsgTools::Triangles::SharedVertex::ValidRefPtr  SharedVertexPtr;
  typedef SharedVertexPtr                                 value_type;
  typedef std::vector< value_type >                       Points;
  typedef Points::iterator                                iterator;
  typedef Points::const_iterator                          const_iterator;

  Loop();
  Loop( const Loop & );
  Loop ( const Points & );
  ~Loop();

  void                  append ( const value_type& v ) { _loop.push_back( v ); }

  bool                  triangulate ( Usul::Interfaces::IUnknown *caller, bool buildOnFly );

  iterator              begin() { return _loop.begin(); }
  iterator              end()   { return _loop.end(); }

  const_iterator        begin() const { return _loop.begin(); }
  const_iterator        end()   const { return _loop.end();   }

  // Clear the loop.
  void                  clear();

  // Is this loop empty?
  bool                  empty() const { return _loop.empty(); }
  
  // Erase element at index i
  void                  erase ( unsigned int i ) { _loop.erase ( _loop.begin() + i ); }

  // Insert elements after pos.
  template < class InputIterator >
  void                  insert ( iterator pos, InputIterator first, InputIterator last );

  // Get the size.
  unsigned int          size() const { return _loop.size(); }

  // Check to see if the given point is inside this loop.
  bool                  pointInside( const osg::Vec3 &, Usul::Interfaces::IUnknown *caller ) const;

  // Add a loop to the list of inner loops.
  void                  addInnerLoop ( const Loop& loop ) { _innerLoops.push_back( loop._loop ); }

  // Does this loop have any inner loops?
  bool                  hasInnerLoop ( ) const { return !_innerLoops.empty(); }

  // Get the number of inner loops.
  unsigned int          numInnerLoops ( ) const { return _innerLoops.size(); }

  // Get the points for inner loop i.
  const Points&         points ( unsigned int i ) const { return _innerLoops.at( i ); }

  // Build frame data from this loop.
  void                  getFrameData ( osg::Vec3&, float &, osg::Quat&, Usul::Interfaces::IUnknown *caller ) const;

  // Get the osg::Vec3 for the i'th point in this loop.
  const osg::Vec3f&     vertex ( unsigned int i, Usul::Interfaces::IUnknown *caller  ) const;

/// This should be public
  int                  isCoplanar( Usul::Interfaces::IUnknown *caller) const;
  void                 printQuakePolygonFile( Usul::Interfaces::IUnknown *caller );

  bool                 valid() const { return _valid; }
  void                 valid( bool b ) { _valid = b; }
  
private:

  // Get the osg::Vec3array from the loop.
  osg::Vec3Array*   _vertices( Usul::Interfaces::IUnknown *caller ) const;

  Points _loop; //Vector of SharedVertices

  typedef std::vector< Points > InnerLoops;

  InnerLoops _innerLoops; // Vector of Points

  bool _valid;

}; // class Loop.


///////////////////////////////////////////////////////////////////////////////
//
//  Insert [ first, last ) before pos.
//
///////////////////////////////////////////////////////////////////////////////

template < class InputIterator >
inline void  Loop::insert ( iterator pos, InputIterator first, InputIterator last )
{
  _loop.insert( pos, first, last );
}

} //namespace Triangles
} //namespace OsgTools


#endif // __EDIT_POLYGONS_LOOP_H__
