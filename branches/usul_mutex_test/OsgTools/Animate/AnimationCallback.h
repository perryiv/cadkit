
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ANIMATION_CALLBACK_H__
#define __ANIMATION_CALLBACK_H__

#include "OsgTools/Export.h"

#include "osg/NodeCallback"

namespace OsgTools {
namespace Animate {

class OSG_TOOLS_EXPORT AnimationCallback : public osg::NodeCallback
{
public:
  typedef osg::NodeCallback BaseClass;

  AnimationCallback();
  virtual ~AnimationCallback();

  virtual void operator() ( osg::Node* node, osg::NodeVisitor* nv );

  void setDuration( float speed ) { _speed = speed; }

  void accumulate ( bool b ) { _accumulate = b; }

private:

  float _speed;
  double _lastTime;
  unsigned int _lastChild;
  bool _accumulate;
};

}
}

#endif // __ANIMATION_CALLBACK_H__
