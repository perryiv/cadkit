
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

#include "GSG/Core/AttributeSet.h"


namespace GSG {


class Shape;
class AttributeSet;


class GSG_CORE_EXPORT Factory : public Referenced
{
public:

  GSG_DECLARE_REFERENCED ( Factory );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Factory, Referenced );

  explicit Factory();
  Factory ( const Factory &f );

  // Get/set the arrtibutes.
  const AttributeSet *  attibutes() const { return _attributes; }
  AttributeSet *        attibutes()       { return _attributes; }
  void                  attributes ( AttributeSet * );

  // Make a box.
  Shape *               box ( const Vec3 &center, const Vec3 &size );

  // Make a cube. The first one makes a unit cube at the origin.
  Shape *               cube();
  Shape *               cube ( const Vec3 &center, Real size );

  // Make a sphere. The first one makes a unit sphere at the origin.
  Shape *               sphere ( UnsignedInteger numSubDivisions = 0 );
  Shape *               sphere ( const Vec3 &center, Real radius, UnsignedInteger numSubDivisions = 0 );

protected:

  virtual ~Factory();

private:

  AttributeSet::Ptr _attributes;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_FACTORY_H_
