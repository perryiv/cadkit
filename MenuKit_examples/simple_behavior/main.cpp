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

#include "osgDB/FileUtils"
#include "MenuKit/OSG/FlatBox.h"

#include "MenuKit/SampleMenu.h"

MenuKit::Menu* create_menu();

int main(int argc,char* argv[])
{
  std::string file;
  if( argc > 1 && osgDB::fileExists(argv[1]) )
    file = argv[1];
  else
    file = "C:\\sdk\\share\\OpenSceneGraph-Data\\fonts\\arial.ttf";

  // make the scene tree
  osg::ref_ptr<osg::Group> root = new osg::Group();
  root->setName("root");

  // "un-spin" the viewer
  osg::Matrix xspin( osg::Matrix::rotate(osg::PI_2,osg::Vec3(1.0,0.0,0.0)) );

  // make a polygon so the viewer can do its initialization scaling
  // and we still have a decent sized world
  MenuKit::OSG::FlatBox fb(20.0,20.0,-0.5,osg::PrimitiveSet::LINE_LOOP);  // push back 0.5 units
  fb.color( osg::Vec4(0.1,0.9,0.1,1.0) );
  osg::ref_ptr<osg::Geode> background = new osg::Geode();
  background->setName("background");
  background->addDrawable( fb() );
  osg::ref_ptr<osg::MatrixTransform> bgxform = new osg::MatrixTransform();
  bgxform->addChild( background.get() );
  bgxform->setMatrix( xspin );
  bgxform->setName( "bgxform" );
  root->addChild( bgxform.get() );

  osg::Matrix offset( osg::Matrix::translate(osg::Vec3(-0.5*fb.width(),0.5*fb.height(),0.0)) );
  osg::ref_ptr<osg::MatrixTransform> menunode = new osg::MatrixTransform();
  menunode->setName("menunode");
  menunode->setMatrix( offset*xspin );
  root->addChild( menunode.get() );

  // make a viewer
  osg::ref_ptr<osgProducer::Viewer> viewer = new osgProducer::Viewer();
  viewer->setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);
  viewer->setSceneData( root.get() );

  // make an eventhandler
  osg::ref_ptr<MenuController> menuhandler = new MenuController( menunode.get(), file );
  menuhandler->behavior()->root( create_menu() );
  menuhandler->behavior()->ensureFocus();

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

MenuKit::Menu* create_menu()
{
  MenuKit::SampleMenu sample;
  return sample();
}
