
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper functor for making the tiles dirty.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _STAR_SYSTEM_DIRTY_TILES_FUNCTOR_CLASS_H_
#define _STAR_SYSTEM_DIRTY_TILES_FUNCTOR_CLASS_H_

#include "StarSystem/Extents.h"

#include "osg/Vec2d"

namespace osg { class Group; }


namespace StarSystem {


struct DirtyTiles
{
public:

  typedef StarSystem::Extents < osg::Vec2d > Extents;

  DirtyTiles ( bool state, unsigned int flags );
  DirtyTiles ( bool state, unsigned int flags, const Extents &extents );
  DirtyTiles ( const DirtyTiles & );

  void operator () ( osg::Group * );

private:

  // No default construction.
  DirtyTiles();

  // No assignment.
  DirtyTiles &operator = ( const DirtyTiles & );

  bool _state;
  unsigned int _flags;
  Extents _extents;
  bool _useExtents;
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_DIRTY_TILES_FUNCTOR_CLASS_H_