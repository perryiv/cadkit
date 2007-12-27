
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_WIDGETS_AXES_H__
#define __OSG_TOOLS_WIDGETS_AXES_H__

#include "OsgTools/Export.h"

#include "osg/MatrixTransform"
#include "osg/NodeCallback"

namespace OsgTools {
namespace Widgets {

class OSG_TOOLS_EXPORT Axes : public osg::MatrixTransform
{
public:
  typedef osg::MatrixTransform BaseClass;
  
  Axes();

protected:

  /// Use reference counting.
  virtual ~Axes();

  class AxesSort : public osg::NodeCallback
  {
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );
  };
};

}
}

#endif // __OSG_TOOLS_WIDGETS_AXES_H__

