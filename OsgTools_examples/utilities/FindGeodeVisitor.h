///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _first_drawable_visitor_h_
#define _first_drawable_visitor_h_

#include "osg/NodeVisitor"
#include "osg/Group"
#include "osg/Geode"
#include "osg/Drawable"

/** FindGeodeVisitor
  * The FirstGeodeVisitor is a helper class for the Display class.
  * It's only purpose is to find the first geode in the scene,
  * that contains a populated list of drawables.
  */
class FindGeodeVisitor : public osg::NodeVisitor
{
public:
  typedef osg::NodeVisitor BaseClass;
  FindGeodeVisitor(): BaseClass(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
    _first(0x0)
  {
  }

  virtual ~FindGeodeVisitor() {}

  virtual void apply(osg::Group& group)
  {
    if( !_first )  // give up if we find a good one
      traverse(group);  // continue with scene traversal
  }

  virtual void apply(osg::Geode& geode)
  {
    // grab only the first valid geode containing drawables
    if( !_first && geode.getNumDrawables()>0 )
      _first = &geode;  // grab a good one
  }

  const osg::Geode* first() const { return _first; }
  osg::Geode* first() { return _first; }

  void reuse() { _first = 0x0; }

private:
  osg::Geode* _first;
};

#endif
