
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

  double radius () const { return _radius; }
  void   radius ( double radius ) { _radius = radius; }
private:
  double _radius;
};

}
}


#endif // __OSGTOOLS_BUILDERS_ARROW_H__

