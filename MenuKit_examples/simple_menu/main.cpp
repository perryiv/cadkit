
#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

#include "MenuKit/StreamVisitor.h"
#include <iostream>

#include "MenuKit/OSG/TileMason.h"

#include "osgProducer/Viewer"
#include "osgUtil/Optimizer"

// prototypes
MenuKit::Menu* create_menu();

int main(unsigned int argc, char* argv[])
{
  MenuKit::Menu::Ptr menu = create_menu();

  MenuKit::StreamVisitor::Ptr streamer = new MenuKit::StreamVisitor(std::cout);
  menu->accept( *streamer );

  // make some color schemes
  osg::Vec4 white(1.0,1.0,1.0,1.0);
  osg::Vec4 darkgrey(0.5,0.5,0.5,1.0);
  osg::Vec4 lightgrey(0.8,0.8,0.8,1.0);
  osg::Vec4 black(0.0,0.0,0.0,1.0);
  osg::Vec4 red(1.0,0.0,0.0,1.0);
  osg::Vec4 blue(0.0,0.0,1.0,1.0);
  osg::Vec4 lightblue(0.5,0.5,1.0,1.0);

  // highlight theme
  MenuKit::OSG::osgColor hil;
  hil.text(black);
  hil.middle(lightblue);
  hil.back(lightblue);
  hil.border(blue);
  hil.special_back(lightblue);
  hil.special(blue);

  // normal theme
  MenuKit::OSG::osgColor nor;
  nor.text(black);
  nor.middle(darkgrey);
  nor.back(lightgrey);
  nor.border(lightgrey);
  nor.special_back(lightblue);
  nor.special(blue);

  // disabled theme
  MenuKit::OSG::osgColor dis(darkgrey,darkgrey,lightgrey,lightgrey,blue,lightblue);

  // make a tile mason
  MenuKit::OSG::osgTileMason::Ptr mason = new MenuKit::OSG::osgTileMason();

  // give the schemes to the tile
  mason->tile().disabled( dis );
  mason->tile().normal( nor );
  mason->tile().highlight( hil );

  // load a nice font
  //osgText::Font* font = osgText::readFontFile("arial.ttf" );

  // give the font to the skin
  //mason->tile().skin().font( font );

  // produce the menu scene
  menu->accept( *mason );
  osg::ref_ptr<osg::Node> scene = mason->scene();

  // optimize the menu
  osgUtil::Optimizer mizer;
  mizer.optimize( scene.get() );

  // place menu graphics into scene
  osg::ref_ptr<osg::MatrixTransform> root = new osg::MatrixTransform();
  root->addChild( scene.get() );

  // (un)spin the model, Viewer class imposes a 'spin' on data
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

MenuKit::Menu* create_menu()
{
  // creation
  MenuKit::Menu* main = new MenuKit::Menu;
  main->layout( MenuKit::Menu::HORIZONTAL );
  main->text( "" );
  main->expanded( true );

  MenuKit::Menu::Ptr file = new MenuKit::Menu();
  file->layout( MenuKit::Menu::VERTICAL );
  file->text( "File" );
  file->expanded( false );

  MenuKit::Menu::Ptr edit = new MenuKit::Menu();
  edit->layout( MenuKit::Menu::VERTICAL );
  edit->text( "Edit" );
  edit->expanded( true );

  MenuKit::Menu::Ptr tools = new MenuKit::Menu();
  tools->layout( MenuKit::Menu::VERTICAL );
  tools->text( "Toolsxxxxxxx" );
  tools->expanded( false );

  MenuKit::Menu::Ptr help = new MenuKit::Menu();
  help->layout( MenuKit::Menu::VERTICAL );
  help->text( "Help" );
  help->expanded( false );

  MenuKit::Menu::Ptr move = new MenuKit::Menu();
  move->layout( MenuKit::Menu::VERTICAL );
  move->text( "Move" );
  move->expanded( true );

  MenuKit::Button::Ptr open = new MenuKit::Button();
  open->text( "Open" );

  MenuKit::Button::Ptr exit = new MenuKit::Button();
  exit->text( "Exit" );

  MenuKit::Button::Ptr cut = new MenuKit::Button();
  cut->text( "Cut" );

  MenuKit::Button::Ptr paste = new MenuKit::Button();
  paste->text( "Pastexxxxxx" );

  MenuKit::Button::Ptr search = new MenuKit::Button();
  search->text( "Search" );

  MenuKit::Button::Ptr index = new MenuKit::Button();
  index->text( "Index" );

  MenuKit::Button::Ptr about = new MenuKit::Button();
  about->text( "About" );

  MenuKit::Button::Ptr xyz = new MenuKit::Button();
  xyz->text( "XYZ" );
  xyz->checked( true );
  xyz->marked( true );
  xyz->toggle( true );

  MenuKit::Button::Ptr rotate = new MenuKit::Button();
  rotate->text( "Rotate" );
  rotate->toggle( true );

  MenuKit::Button::Ptr free = new MenuKit::Button();
  free->text( "Free" );
  free->toggle( true );

  MenuKit::Button::Ptr scale = new MenuKit::Button();
  scale->text( "Scale" );
  scale->toggle( true );

  // organize
  main->append( file.get() );
  main->append( edit.get() );
  main->append( tools.get() );
  main->append( help.get() );

  file->append( open.get() );
  file->append( exit.get() );

  edit->append( cut.get() );
  edit->append( paste.get() );
  edit->append( move.get() );

  tools->append( search.get() );

  help->append( index.get() );
  help->append( about.get() );

  move->append( xyz.get() );
  move->append( rotate.get() );
  move->append( free.get() );
  move->append( scale.get() );

  return( main );
}
