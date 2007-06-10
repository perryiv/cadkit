/** \file main.cpp
  * \title Tile Example
  * \description This example shows how to use the Tile class
  * to generate a graphic for a MenuKit::Item
  */

#include "osg/Group"
#include "osg/Projection"
#include "osg/MatrixTransform"
#include "osg/Matrix"
#include "osgProducer/Viewer"
#include "osgDB/FileUtils"
#include "osgText/Font"

#include "MenuKit/OSG/VisualThemeSkin.h"
#include "MenuKit/OSG/ThemeSkinTile.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

#include "MenuKit/Box.h"

int main(int argc, char* argv[])
{
  // screen dimensions to be used for menu display
  MenuKit::Detail::Box screen(768.0,1024.0);

  //--- allocate a skin ---//
  typedef MenuKit::OSG::VisualThemeSkin aSkin;
  aSkin::Ptr skin = new aSkin();

  //--- setup the skin ---//
  // adjust some default values
  skin->letter_height( 0.04*screen.height() );
  skin->margin( 0.3*skin->letter_height() );
  skin->border( 0.05*skin->letter_height() );

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
  tile->mode(MenuKit::OSG::Tile::NORMAL);
  //--- --------------- ---//

  //--- define a menu item ---//
  MenuKit::Menu::Ptr file = new MenuKit::Menu("File","",MenuKit::Menu::VERTICAL);
  file->expanded( true );
  //MenuKit::Menu::Ptr button = new MenuKit::Menu("Open","",MenuKit::Menu::VERTICAL);
  //button->expanded( true );
  MenuKit::Button::Ptr button = new MenuKit::Button("Open","");
  button->toggle( true );
  //button->radio( true );
  //button->checked( true );
  button->enabled( false );
  file->append( button.get() );

  //--- generate the graphic for the item ---//
  // set the size for the tile
  tile->box( Detail::Box(tile->height(*button), tile->width(*button)) );
  osg::ref_ptr<osg::Node> scene = tile->operator ()( *button );
  //--- --------------------------------- ---//

  //--- place menu graphics into the environment ---//
  // make a transform node for attaching the menu graphics
  osg::ref_ptr<osg::MatrixTransform> xform = new osg::MatrixTransform();
  xform->addChild( scene.get() );
  osg::Matrix rotate(osg::Matrix::rotate(osg::PI_2, osg::Vec3(1.0f,0.0f,0.0f)));
  osg::Matrix translate(osg::Matrix::translate(osg::Vec3(-0.5*screen.width(),0.5*screen.height(),0.0)));
  xform->setMatrix( /*translate**/rotate );

  // attach the transform node to the HUD
  osg::ref_ptr<osg::Projection> hud = new osg::Projection();
  hud->setMatrix( osg::Matrix::ortho2D(-0.5*screen.width(),0.5*screen.width(),-0.5*screen.height(),0.5*screen.height()) );
  hud->addChild( xform.get() );

  // configure the OpenGL settings for this menu node
  osg::ref_ptr<osg::StateSet> ss = hud->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
  ss->setRenderBinDetails(10,"RenderBin"); // a high number makes it render last

  // make some background graphics to fight osgProducer::Viewer's auto-centering
  MenuKit::OSG::Border border( screen, MenuKit::Detail::Box(0.8*screen.height(),0.8*screen.width()), 0.0 );
  border.color( osg::Vec4(1.0,0.0,0.0,1.0) );
  osg::ref_ptr<osg::Node> brdr = border();
  osg::ref_ptr<osg::MatrixTransform> bg = new osg::MatrixTransform();
  bg->addChild( brdr.get() );
  bg->setMatrix( osg::Matrix::rotate( osg::PI_2, osg::Vec3(1.0,0.0,0.0) ) );

  // attach the absolute transform node to the scene root
  osg::ref_ptr<osg::Group> root = new osg::Group();
  root->addChild( hud.get() );
  root->addChild( bg.get() );

  // make a viewer
  osg::ref_ptr<osgProducer::Viewer> viewer = new osgProducer::Viewer();
  viewer->setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);

  // add the scene root to the viewer's data
  viewer->setSceneData( root.get() );

  // begin the render loop
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
