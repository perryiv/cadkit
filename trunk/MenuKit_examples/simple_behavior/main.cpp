/** \file main.cpp
  * \title Behavior Example
  * \description This example shows how to use the Behavior class
  * from a GUI interface to navigate a menu.
  */

#include "MenuController.h"               // to control the scene

#include "MenuKit/OSG/VisualThemeSkin.h"  // for skin
#include "MenuKit/OSG/ThemeSkinTile.h"    // for tile
#include "MenuKit/Behavior.h"             // for behavior

#include "MenuKit/SampleMenu.h"           // for content
#include "MenuKit/CloseVisitor.h"         // to initialize

#include "osgProducer/Viewer"             // for viewing entire scene
#include "osg/Projection"                 // for placing the menu subgraph in an orthographic projection
#include "osgDB/FileUtils"


int main(unsigned int argc, char* argv[])
{
  //--- allocate a skin ---//
  typedef MenuKit::OSG::VisualThemeSkin aSkin;
  aSkin::Ptr skin = new aSkin();
  skin->letter_height( 50.0f );

  //--- setup the skin ---//
  // load font for skin
  std::string fontfile("");
  if( argc > 1 && osgDB::fileExists(argv[1]) )
    fontfile = argv[1];
  else
    fontfile = "C:\\sdk\\share\\OpenSceneGraph-Data\\fonts\\arial.ttf";
  osg::ref_ptr<osgText::Font> font = osgText::readFontFile( fontfile );
  if( font.valid() )
    skin->font( font.get() );
  //--- --------------- ---//

  //--- allocate a tile ---//
  typedef MenuKit::OSG::SkinTile<aSkin::base_class> aTile;
  aTile::Ptr tile = new aTile();

  //--- setup the tile ---//
  tile->skin( skin.get() );  // no more skin code after here
  //--- --------------- ---//

  // produce the menu
  MenuKit::SampleMenu sample;
  MenuKit::Menu::Ptr menu = sample();

  // make sure it is closed upon initial view
  {
    MenuKit::CloseVisitor::Ptr collapser = new MenuKit::CloseVisitor();
    menu->accept( *collapser );
  }

  // allocate a Behavior instance and provide the menu to operate upon
  MenuKit::Behavior::Ptr behavior = new MenuKit::Behavior();
  behavior->root( menu.get() );
  behavior->moveFocused( MenuKit::Behavior::DOWN );

  // make a mason
  MenuController::TileMason::Ptr mason = new MenuController::TileMason();
  mason->tile( tile.get() );

  // screen dimensions to be used for menu display
  MenuKit::Detail::Box area(1024.0,1280.0);

  //--- place menu graphics into the environment ---//
  // make a transform node for attaching the menu graphics
  osg::ref_ptr<osg::MatrixTransform> xform = new osg::MatrixTransform();
  osg::Matrix rotate(osg::Matrix::rotate(osg::PI_2, osg::Vec3(1.0f,0.0f,0.0f)));
  osg::Matrix translate(osg::Matrix::translate(osg::Vec3(-0.5*area.width(),0.5*area.height(),0.0)));
  xform->setMatrix( translate*rotate );

  // attach the transform node to the HUD
  osg::ref_ptr<osg::Projection> hud = new osg::Projection();
  hud->setMatrix( osg::Matrix::ortho2D(-0.5*area.width(),0.5*area.width(),-0.5*area.height(),0.5*area.height()) );
  hud->addChild( xform.get() );

  // configure the OpenGL settings for this menu node
  osg::ref_ptr<osg::StateSet> ss = hud->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
  ss->setRenderBinDetails(10,"RenderBin"); // a high number makes it render last

  // make some background graphics to fight osgProducer::Viewer's auto-centering
  MenuKit::OSG::Border border( MenuKit::Detail::Box(1.1*area.height(),1.1*area.width()), area, -1.0 );
  border.color( osg::Vec4(1.0,0.0,0.0,1.0) );
  osg::ref_ptr<osg::Node> brdr = border();
  osg::ref_ptr<osg::MatrixTransform> bg = new osg::MatrixTransform();
  bg->addChild( brdr.get() );
  bg->setMatrix( osg::Matrix::rotate( osg::PI_2, osg::Vec3(1.0,0.0,0.0) ) );

  // attach the absolute transform node to the scene root
  osg::ref_ptr<osg::Group> root = new osg::Group();
  root->addChild( hud.get() );
  root->addChild( bg.get() );

  // make an EventHandler to control the menu display
  osg::ref_ptr<MenuController> mc = new MenuController();
  mc->attach_node( xform.get() );
  mc->behavior( behavior.get() );
  mc->mason( mason.get() );

  // make a viewer
  osg::ref_ptr<osgProducer::Viewer> viewer = new osgProducer::Viewer();
  viewer->setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);

  // add the event handler
  viewer->getEventHandlerList().push_front( mc.get() );

  // set the scene data
  viewer->setSceneData( root.get() );

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
