/** main.cpp - Behavior Example
  * This example shows how to use the 'Behavior' class
  * from a GUI interface to navigate a menu.
  */

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

#include "osgProducer/Viewer"
#include "osg/Group"
#include "osg/MatrixTransform"

#include "MenuController.h"
#include "GraphStreamVisitor.h"

#include "create.h"

int main(int argc,char* argv[])
{
  // make the scene tree
  osg::ref_ptr<osg::Group> root = new osg::Group();
  root->setName("root");

  osg::ref_ptr<osg::MatrixTransform> menunode( new osg::MatrixTransform() );
  menunode->setName("menunode");
  osg::Matrix xspin( osg::Matrix::rotate(osg::PI_2,osg::Vec3(1.0,0.0,0.0)) );
  osg::Matrix yspin( osg::Matrix::rotate(osg::PI,osg::Vec3(0.0,1.0,0.0)) );
  osg::Matrix total = yspin * xspin;
  menunode->setMatrix( total );
  root->addChild( menunode.get() );

  // make a viewer
  osg::ref_ptr<osgProducer::Viewer> viewer = new osgProducer::Viewer();
  viewer->setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);
  viewer->setSceneData( root.get() );

  // make an eventhandler
  osg::ref_ptr<MenuController> menuhandler = new MenuController( menunode.get() );
  create_menu cm;
  menuhandler->behavior()->root( cm() );

  // give the eventhandler to the viewer
  viewer->getEventHandlerList().push_front( menuhandler.get() );

  // begin viewing
  viewer->realize();
  while( !viewer->done() )
  {
    viewer->sync();
    viewer->update();
    viewer->frame();
  }
  viewer->sync();

  return 1;
}
