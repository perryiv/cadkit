
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to make a ray.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_RAY_BUILDER_H_
#define _OSG_TOOLS_RAY_BUILDER_H_

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"

#include "osg/Vec4"
#include "osg/Vec3"


namespace OsgTools {


class OSG_TOOLS_EXPORT Ray
{
public:

  Ray();

  void                color ( const osg::Vec4 &c ) { _color = c; }
  const osg::Vec4 &   color() const { return _color; }

  void                start ( const osg::Vec3 &s ) { _start = s; }
  const osg::Vec3 &   start() const { return _start; }

  void                end ( const osg::Vec3 &e ) { _end = e; }
  const osg::Vec3 &   end() const { return _end; }

  void                thickness ( float t ) { _thickness = t; }
  const float &       thickness() const { return _thickness; }

  osg::Node *         operator()() const;

private:

  osg::Vec4 _color;
  osg::Vec3 _start;
  osg::Vec3 _end;
  float _thickness;
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_RAY_BUILDER_H_
