
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A node factory.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_FACTORY_H_
#define _GENERIC_SCENE_GRAPH_CORE_FACTORY_H_

#include "GSG/Core/Referenced.h"


namespace GSG {

class Shape;
class Vec4Pool;
class Vec3Pool;
class Vec2Pool;


class GSG_CORE_EXPORT Factory : public Referenced
{
public:

  GSG_DECLARE_CLONE ( Factory );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Factory, Referenced );
  typedef Vec3Pool VertexPool;
  typedef Vec3Pool NormalPool;
  typedef Vec4Pool ColorPool;

  explicit Factory();
  Factory ( const Factory &f );

  // Make a box.
  Shape *               box ( const Vec3 &center, const Vec3 &size );

  // Make a cube. The first one makes a unit cube at the origin.
  Shape *               cube();
  Shape *               cube ( const Vec3 &center, Real size );

  // Make a sphere. The first one makes a unit sphere at the origin.
  Shape *               sphere ( UnsignedInteger numSubDivisions = 4 );
  Shape *               sphere ( const Vec3 &center, Real radius, UnsignedInteger numSubDivisions = 4 );

protected:

  virtual ~Factory();
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_FACTORY_H_
