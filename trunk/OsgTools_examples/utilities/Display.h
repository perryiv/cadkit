///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _app_display_h_
#define _app_display_h_

#include "FindGeodeVisitor.h"
#include "OsgTools/Dimension.h"

#include "osgGA/GUIEventHandler"
#include "osgProducer/Viewer"

#include <cassert>

namespace osg
{
  class Node;
  class Geode;
};

/** Display
  * Display is a class that will examine the
  * scene data for the viewer it is associated with.
  */
class Display : public osgGA::GUIEventHandler
{
public:
  typedef osgGA::GUIEventHandler BaseClass;
  Display(osgProducer::Viewer& v): BaseClass(), _viewer(v)
  {
    this->append_viewer();
  }

  virtual ~Display() {}

  void append_viewer()
  {
    osgProducer::Viewer::EventHandlerList& list = _viewer.getEventHandlerList();
    list.push_back( this );
  }

  virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

  const osgProducer::Viewer& viewer() const { return _viewer; }
  void viewer(osgProducer::Viewer& v)
  {
    // append itself to the viewer list
    _viewer = v;
    append_viewer();
  }

protected:
  osg::Geode* find_geode(osg::Node* n);

private:
  Display();  // not implemented by design
  osgProducer::Viewer& _viewer;
};

#endif
