
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_UTILITIES_TRANSLATE_GEOMETRY_H__
#define __OSG_TOOLS_UTILITIES_TRANSLATE_GEOMETRY_H__

#include "OsgTools/Export.h"

#include "osg/Array"
#include "osg/NodeVisitor"
#include "osg/ref_ptr"

#include "osg/Vec3d"

#include <set>

namespace OsgTools {
namespace Utilities {

class OSG_TOOLS_EXPORT TranslateGeometry : public osg::NodeVisitor
{
public:
  typedef osg::NodeVisitor BaseClass;

  TranslateGeometry ( const osg::Vec3d& amount );

  virtual void apply( osg::Geode& geode );

protected:

  virtual ~TranslateGeometry();

private:
  typedef osg::ref_ptr<osg::Vec3Array> VerticesPtr;
  typedef std::set<VerticesPtr> VerticesProcessed;

  osg::Vec3d _amount;

  VerticesProcessed _vertices;
};


}
}

#endif // __OSG_TOOLS_UTILITIES_TRANSLATE_GEOMETRY_H__
