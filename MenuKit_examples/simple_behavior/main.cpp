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

MenuKit::Menu* create_menu();

int main(int argc,char* argv[])
{
  std::string file;
  if( argc > 1 && osgDB::fileExists(argv[1]) )
    file = argv[1];
  else
    file = "C:\\sdk\\share\\OpenSceneGraph-Data\\fonts\\dirtydoz.ttf";

  // make the scene tree
  osg::ref_ptr<osg::Group> root = new osg::Group();
  root->setName("root");

  osg::ref_ptr<osg::MatrixTransform> menunode = new osg::MatrixTransform();
  menunode->setName("menunode");
  osg::Matrix xspin( osg::Matrix::rotate(osg::PI_2,osg::Vec3(1.0,0.0,0.0)) );
  osg::Matrix yspin( osg::Matrix::rotate(osg::PI,osg::Vec3(0.0,1.0,0.0)) );
  osg::Matrix total = yspin * xspin;
  menunode->setMatrix( xspin );
  root->addChild( menunode.get() );

  MenuKit::OSG::FlatBox fb(20.0,20.0,0.5);
  fb.color( osg::Vec4(0.1,0.9,0.1,1.0) );
  osg::ref_ptr<osg::Geode> background = new osg::Geode();
  background->setName("background");
  background->addDrawable( fb() );
  osg::ref_ptr<osg::MatrixTransform> bgxform = new osg::MatrixTransform();
  bgxform->addChild( background.get() );
  bgxform->setMatrix( total );
  bgxform->setName( "bgxform" );
  root->addChild( bgxform.get() );

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
  MenuKit::Button::Ptr spacer = new MenuKit::Button("Spacer","");
  spacer->separator( true );

  MenuKit::Button::Ptr newproject = new MenuKit::Button("Project","");
  MenuKit::Button::Ptr newfile = new MenuKit::Button("File","");
  MenuKit::Menu::Ptr newstuff = new MenuKit::Menu("New","",MenuKit::Menu::VERTICAL);
  newstuff->append( newproject.get() );
  newstuff->append( newfile.get() );

  MenuKit::Button::Ptr openproject = new MenuKit::Button("Project","");
  MenuKit::Button::Ptr openfile = new MenuKit::Button("File","");
  MenuKit::Menu::Ptr open = new MenuKit::Menu("Open","",MenuKit::Menu::VERTICAL);
  open->append( openproject.get() );
  open->append( openfile.get() );

  MenuKit::Button::Ptr close = new MenuKit::Button("Close","");
  MenuKit::Button::Ptr exit = new MenuKit::Button("Exit","");

  MenuKit::Button::Ptr undo = new MenuKit::Button("Undo","");
  MenuKit::Button::Ptr redo = new MenuKit::Button("Redo","");
  redo->enabled( false );
  MenuKit::Button::Ptr copy = new MenuKit::Button("Copy","");
  MenuKit::Button::Ptr paste = new MenuKit::Button("Paste","");

  MenuKit::Button::Ptr solutionbrowser = new MenuKit::Button("Solution Browser","");
  solutionbrowser->toggle( true );
  MenuKit::Button::Ptr classbrowser = new MenuKit::Button("Class Browser","");
  classbrowser->toggle( true );

  MenuKit::Button::Ptr standardtoolbar = new MenuKit::Button("Standard","");
  standardtoolbar->toggle( true );
  MenuKit::Button::Ptr texteditortoolbar = new MenuKit::Button("Text Editor","");
  texteditortoolbar->toggle( true );
  MenuKit::Button::Ptr formattoolbar = new MenuKit::Button("Format","");
  formattoolbar->toggle( true );
  MenuKit::Menu::Ptr toolbars = new MenuKit::Menu("Toolbars","",MenuKit::Menu::VERTICAL);
  toolbars->append( formattoolbar.get() );
  toolbars->append( standardtoolbar.get() );
  toolbars->append( texteditortoolbar.get() );

  MenuKit::Menu::Ptr file = new MenuKit::Menu("File","",MenuKit::Menu::VERTICAL);
  file->append( newstuff.get() );
  file->append( open.get() );
  file->append( close.get() );
  file->append( spacer.get() );
  file->append( exit.get() );

  MenuKit::Menu::Ptr edit = new MenuKit::Menu("Edit","",MenuKit::Menu::VERTICAL);
  edit->append( undo.get() );
  edit->append( redo.get() );
  edit->append( copy.get() );
  edit->append( paste.get() );

  MenuKit::Menu::Ptr view = new MenuKit::Menu("View","",MenuKit::Menu::VERTICAL);
  view->append( toolbars.get() );
  view->append( solutionbrowser.get() );
  view->append( classbrowser.get() );

  MenuKit::Menu* top = new MenuKit::Menu("","",MenuKit::Menu::HORIZONTAL);
  top->append( file.get() );
  top->append( edit.get() );
  top->append( view.get() );
  return top;
}
