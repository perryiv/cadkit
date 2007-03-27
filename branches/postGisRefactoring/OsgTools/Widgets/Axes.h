
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
#include "OsgTools/Draggers/Translate2.h"

#include "osg/NodeCallback"

namespace OsgTools {
namespace Widgets {

class OSG_TOOLS_EXPORT Axes : public OsgTools::Draggers::Translate2
{
public:
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

