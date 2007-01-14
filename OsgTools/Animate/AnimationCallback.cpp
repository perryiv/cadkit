
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Animate/AnimationCallback.h"

#include "osg/FrameStamp"
#include "osg/NodeVisitor"
#include "osg/Group"

using namespace OsgTools::Animate;

AnimationCallback::AnimationCallback() : BaseClass(),
_speed ( 0.0 ),
_lastTime ( -1 ),
_lastChild ( 0 ),
_accumulate ( true )
{
}

AnimationCallback::~AnimationCallback()
{
}

void AnimationCallback::operator() ( osg::Node* node, osg::NodeVisitor* nv )
{
  if( osg::Group *group = dynamic_cast < osg::Group* > ( node ) )
  {
    if( 0 == group->getNumChildren() )
      return;

    const osg::FrameStamp* framestamp = nv->getFrameStamp();
    if (framestamp)
    {
      double t = framestamp->getReferenceTime();

      if( _lastTime == -1 )
        _lastTime = t;

      double duration ( t - _lastTime );

      if ( duration > _speed )
      {
        ++_lastChild;
        _lastTime = t;
      }

      if( _lastChild >= group->getNumChildren() )
        _lastChild = 0;

      if( _accumulate )
      {
        for( unsigned int i = 0; i < _lastChild; ++i )
        {
          group->getChild(i)->accept(*nv);
        }
      }
      
      group->getChild(_lastChild)->accept(*nv);
    }
  }
}

