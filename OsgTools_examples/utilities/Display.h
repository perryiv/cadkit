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

/** Display
  * Display is a class that will examine the
  * scene data for the viewer it is associated with.
  */
class Display : public osgGA::GUIEventHandler
{
public:
  typedef osgGA::GUIEventHandler BaseClass;
  Display(osgProducer::Viewer* v): BaseClass(), _viewer(v)
  {
    this->append_viewer(v);
  }

  virtual ~Display() {}

  void append_viewer(osgProducer::Viewer* v)
  {
    osgProducer::Viewer::EventHandlerList& list = v->getEventHandlerList();
    list.push_front( this );
  }

  virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
  {
    // anytime there is a keypress, do the calculations
    if( ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN )
    {
      // grab the scene data
      osg::ref_ptr<osg::Node> scene = _viewer->getSceneData();

      // find a geode
      osg::ref_ptr<osg::Geode> g = this->find_geode(scene.get());

      // grab the first drawable
      if( g->getNumDrawables() < 1 )
      {
        /// TODO: use Perry's exception code here
        assert( 0 );
      }

      osg::ref_ptr<osg::Drawable> d( g->getDrawable(1) );

      // find the size of one drawable
      OsgTools::Dimension dim;
      dim.strategy(OsgTools::Dimension::X);
      float width = dim(d.get());
      std::cout << "the drawable is " << width << " units wide." << std::endl;

      // find the size of the largest drawable
    }

    return true;
  }

  osg::Geode* find_geode(osg::Node* n)
  {
    // traverse the scene to find the first drawable
    osg::ref_ptr<FindGeodeVisitor> fgv = new FindGeodeVisitor();
    n->accept( *fgv );
    osg::ref_ptr<osg::Geode> g = fgv->first();
    if( !g.valid() )
    {
      /// TODO: use Perry's exception code to make a message
      /// or issue another search
      assert( 0 );
    }
    return g.release();
  }

  const osgProducer::Viewer* viewer() const { return _viewer.get(); }
  void viewer(osgProducer::Viewer* v)
  {
    // append itself to the viewer list
    _viewer = v;
    append_viewer(_viewer.get());
  }

private:
  Display();  // not implemented by design
  osg::ref_ptr<osgProducer::Viewer> _viewer;
};

#endif
