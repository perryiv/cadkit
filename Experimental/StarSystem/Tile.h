
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
//  A recursive tile.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _STAR_SYSTEM_RECURSIVE_TILE_CLASS_H_
#define _STAR_SYSTEM_RECURSIVE_TILE_CLASS_H_

#include "StarSystem/Export.h"

#include "osg/LOD"

class ossimEllipsoid;


namespace StarSystem {


class STAR_SYSTEM_EXPORT Tile : public osg::LOD
{
public:

  // Useful typedefs.
  typedef osg::LOD BaseClass;

  // OSG Plumbing.
  META_Node ( osg, Tile );

  // Constructors.
  Tile();
  Tile ( const Tile &, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY );

protected:

  // Use reference counting.
  virtual ~Tile();

private:

  // No assignment.
  Tile &operator = ( const Tile & );

  void                      _destroy();

  ossimEllipsoid *_ellipsoid;
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_RECURSIVE_TILE_CLASS_H_
