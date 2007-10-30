
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_EXTENTS_H__
#define __STAR_SYSTEM_EXTENTS_H__

#include "StarSystem/Export.h"

#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "osg/Vec2d"

namespace StarSystem {


class STAR_SYSTEM_EXPORT Extents
{
public:
  typedef osg::Vec2d Vertex;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;

  /// Construction/Destruction.
  Extents();
  Extents( const Vertex& min, const Vertex& max );
  Extents( const Extents& rhs );
  ~Extents();

  /// Assignment.
  Extents& operator= ( const Extents& rhs );

  /// Get the min.
  Vertex               min() const;

  /// Get the max.
  Vertex               max() const;

  /// Expand by the extents.
  void                 expand ( const Extents& extents );

  /// Does the extent intersect this extent.
  bool                 intersects ( const Extents& extents ) const;

  /// Get the mutex
  Mutex&               mutex() const { return *_mutex; }
private:
  void                 _destroy ();

  mutable Mutex *_mutex;
  Vertex _min;
  Vertex _max;
};


}


#endif // __STAR_SYSTEM_EXTENTS_H__
