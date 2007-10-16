
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

#include "Usul/Base/Typed.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Group"

#include <typeinfo>

namespace OsgTools { class Mesh; }
class ossimEllipsoid;


namespace StarSystem {


class STAR_SYSTEM_EXPORT Tile : public osg::Group
{
public:

  // OSG Plumbing.
  META_Node ( osg, Tile );

  // Declare smart pointers.
  USUL_DECLARE_REF_POINTERS ( Tile );

  // Type information.
  USUL_DECLARE_TYPE_ID ( Tile );

  // Useful typedefs.
  typedef osg::Group BaseClass;

  // Constructors.
  Tile ( osg::Vec2d &mn = osg::Vec2d ( 0, 0 ), 
         osg::Vec2d &mx = osg::Vec2d ( 1, 1 ), 
         unsigned int numRows = 10,
         unsigned int numColumns = 10,
         double elevation = 0,
         double distance = 1,
         ossimEllipsoid *ellipsoid = 0x0 );
  Tile ( const Tile &, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY );

  // Traverse the children.
  virtual void              traverse ( osg::NodeVisitor & );

protected:

  // Use reference counting.
  virtual ~Tile();

  // Initialize the tile.
  void                      _init();

private:

  // No assignment.
  Tile &operator = ( const Tile & );

  void                      _destroy();

  ossimEllipsoid *_ellipsoid;
  osg::Vec2d _min;
  osg::Vec2d _max;
  double _elevation;
  double _distance;
  OsgTools::Mesh *_mesh;
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_RECURSIVE_TILE_CLASS_H_
