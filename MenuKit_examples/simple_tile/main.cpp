#include "osg/Group"
#include "osgProducer/Viewer"
#include "osg/ArgumentParser"

#include <iostream>
#include "MenuKit/StreamVisitor.h"
#include "MenuKit/CloseVisitor.h"
#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/OSG/GFX.h"

// prototypes
MenuKit::Menu::Ptr create_menu();

int main(int argc, char* argv[])
{
  // form a menu system
  MenuKit::Menu::Ptr menu = create_menu();

  // show off a visitor
  //MenuKit::CloseVisitor::Ptr closer = new MenuKit::CloseVisitor;
  //menu->accept(*closer);
  MenuKit::StreamVisitor::Ptr streamer = new MenuKit::StreamVisitor(std::cout);
  menu->accept(*streamer);

  // load font for skin
  osg::ref_ptr<osgText::Font> font = osgText::readFontFile("fonts/arial.ttf");

  // load a skin (default skin shown here)
  MenuKit::OSG::Skin::Ptr skin = new MenuKit::OSG::Skin( font.get() );

  // tweek color schemes for the skin

  // make the graphics manufacturer
  MenuKit::OSG::GFX::Ptr gfx = new MenuKit::OSG::GFX( skin.get(),menu.get() );

  // make the menu graphics
  osg::ref_ptr<osg::Node> menu_gfx = (*gfx)( menu.get() );

  // place menu gfx into scene
  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild( menu_gfx.get() );

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

  return 0;
}

MenuKit::Menu::Ptr create_menu()
{
  // creation
  MenuKit::Menu::Ptr main = new MenuKit::Menu;
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
  xyz->toggle( true );
  xyz->checked( true );

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
