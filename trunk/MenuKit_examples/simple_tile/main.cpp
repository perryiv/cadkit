
//#ifdef _WIN32
//# pragma warning ( disable : 4541 ) // warning about multiple inheritance within osg
//#endif

#include "osg/MatrixTransform"
#include "osgProducer/Viewer"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/ColorThemeSkinTile.h"

int main(int argc, char* argv[])
{
  // load font for skin
  osg::ref_ptr<osgText::Font> font = osgText::readFontFile("fonts/arial.ttf");

  // make a skin
  MenuKit::OSG::osgColorThemeSkin osgskin;
  osgskin.font( font.get() );

  // define some colors
  osg::Vec4 red(1.0,0.0,0.0,1.0);
  osg::Vec4 green(0.0,1.0,0.0,1.0);
  osg::Vec4 blue(0.0,0.0,1.0,1.0);
  osg::Vec4 black(0.0,0.0,0.0,1.0);
  osg::Vec4 white(1.0,1.0,1.0,1.0);
  osg::Vec4 transwhite(1.0,1.0,1.0,0.5);

  // define some themes
  MenuKit::OSG::osgColorTheme redblue;
  redblue.text( blue );
  redblue.middle( white );
  redblue.back( red );

  MenuKit::OSG::osgColorTheme redgreen;
  redgreen.text( green  );
  redgreen.middle( black );
  redgreen.back( red );

  MenuKit::OSG::osgColorTheme blackwhite;
  blackwhite.back( black );
  blackwhite.middle( red );
  blackwhite.text( transwhite );

  // define a skin
  MenuKit::OSG::osgColorThemeSkin skinner;

  // define a tile
  MenuKit::OSG::osgColorThemeSkinTile tile;
  tile.skin( skinner );
  tile.normal( blackwhite);
  tile.highlight( redgreen );
  tile.disabled( redblue  );
  tile.box().height( 1.0 );

  // define a menu item
  MenuKit::Menu::Ptr button = new MenuKit::Menu();
  button->text( "Filexxxxxxxxxxxxxx" );
  //button->expanded( true );
  //button->radio( true );
  //button->checked( true );

  // generate the graphic for the item
  tile.box().width( tile.skin().width( *button ) );
  osg::ref_ptr<osg::Node> scene = tile( *button );

  // place menu gfx into scene
  osg::ref_ptr<osg::MatrixTransform> root = new osg::MatrixTransform();
  root->addChild( scene.get() );

  // spin the model
  root->setMatrix( osg::Matrix::rotate( osg::DegreesToRadians(90.0),
                                        osg::Vec3(1.0,0.0,0.0)) );

  // make a viewer
  osg::ref_ptr<osgProducer::Viewer> viewer = new osgProducer::Viewer();
  viewer->setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);

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
