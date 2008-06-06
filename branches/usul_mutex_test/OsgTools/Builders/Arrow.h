
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_BUILDERS_ARROW_H__
#define __OSGTOOLS_BUILDERS_ARROW_H__

#include "OsgTools/Export.h"

#include "osg/Vec3"

#include "OsgTools/Ray.h"


namespace OsgTools {
namespace Builders {


class OSG_TOOLS_EXPORT Arrow : public Ray
{
public:
  typedef Ray BaseClass;

  Arrow();

  osg::Node* operator () () const;

  /// Get/Set radius of the cone.
  double radius () const { return _radius; }
  void   radius ( double radius ) { _radius = radius; }

  /// Get/Set height of the cone.
  double height () const { return _height; }
  void   height ( double height ) { _height = height; }

  /// Get/Set tessellation hints.
  double tessellation () const { return _tessellation; }
  void   tessellation ( double t ) { _tessellation = t; }
private:
  double _radius;
  double _height;
  double _tessellation;
};

}
}


#endif // __OSGTOOLS_BUILDERS_ARROW_H__

