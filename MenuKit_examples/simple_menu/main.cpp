
#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/OSG/VisualThemeSkin.h"  // for skin
#include "MenuKit/OSG/ThemeSkinTile.h"    // for tile
#include "MenuKit/OSG/TileMason.h"        // for mason

#include "MenuKit/StreamVisitor.h"

#include "osgProducer/Viewer"
#include "osgUtil/Optimizer"
#include "osgDB/FileUtils"

// prototypes
MenuKit::Menu* create_menu();

int main(unsigned int argc, char* argv[])
{
  // load font for skin
  std::string fontfile("");
  if( argc > 1 && osgDB::fileExists(argv[1]) )
    fontfile = argv[1];
  else
    fontfile = "C:\\sdk\\share\\OpenSceneGraph-Data\\fonts\\arial.ttf";

  osg::ref_ptr<osgText::Font> font = osgText::readFontFile( fontfile );

  // make a skin
  MenuKit::OSG::VisualThemeSkin::Ptr skin = new MenuKit::OSG::VisualThemeSkin();
  if( font.valid() )
    skin->font( font.get() );

  // make some color schemes
  osg::Vec4 white(1.0,1.0,1.0,1.0);
  osg::Vec4 darkgrey(0.3,0.3,0.3,1.0);
  osg::Vec4 lightgrey(0.8,0.8,0.8,1.0);
  osg::Vec4 black(0.0,0.0,0.0,1.0);
  osg::Vec4 red(1.0,0.0,0.0,1.0);
  osg::Vec4 blue(0.0,0.0,1.0,1.0);
  osg::Vec4 green(0.0,1.0,0.0,1.0);
  osg::Vec4 lightblue(0.5,0.5,1.0,1.0);

  MenuKit::OSG::osg_color_theme ct;
  MenuKit::OSG::osg_color_map norm=ct.get_map();
  MenuKit::OSG::osg_color_map hi=ct.get_map();
  MenuKit::OSG::osg_color_map dis=ct.get_map();

  norm["text"] = blue;
  hi["text"] = red;
  dis["text"] = green;

  // give the themes to the tile
  typedef MenuKit::OSG::osgThemeSkinTile::DisplayModeThemeMap DMTMAP;
  DMTMAP dmtmap;
  dmtmap.insert( DMTMAP::value_type(MenuKit::OSG::TileFunctor::NORMAL,norm) );
  dmtmap.insert( DMTMAP::value_type(MenuKit::OSG::TileFunctor::DISABLED,dis) );
  dmtmap.insert( DMTMAP::value_type(MenuKit::OSG::TileFunctor::HIGHLIGHT,hi) );

  // make a tile
  MenuKit::OSG::osgThemeSkinTile::Ptr tile = new MenuKit::OSG::osgThemeSkinTile();
  tile->theme_map( dmtmap );
  tile->skin( skin.get() );

  // produce the menu
  MenuKit::Menu::Ptr menu = create_menu();

  // output menu to stream
  MenuKit::StreamVisitor::Ptr streamer = new MenuKit::StreamVisitor(std::cout);
  menu->accept( *streamer );

  // make a mason
  MenuKit::OSG::osgMason::Ptr mason = new MenuKit::OSG::osgMason();
  mason->tile( tile.get() );

  // create the scene representing the menu
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
  MenuKit::Menu* top = new MenuKit::Menu;
  top->layout( MenuKit::Menu::HORIZONTAL );
  top->text( "" );
  top->expanded( true );

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

  MenuKit::Button::Ptr spacer = new MenuKit::Button();
  spacer->separator( true );

  MenuKit::Button::Ptr search = new MenuKit::Button();
  search->text( "Search" );

  MenuKit::Button::Ptr index = new MenuKit::Button();
  index->text( "Index" );

  MenuKit::Button::Ptr about = new MenuKit::Button();
  about->text( "About" );

  MenuKit::Button::Ptr xyz = new MenuKit::Button();
  xyz->text( "XYZ" );
  xyz->checked( false );
  xyz->marked( true );
  xyz->toggle( true );

  MenuKit::Button::Ptr rotate = new MenuKit::Button();
  rotate->text( "Rotate" );
  rotate->toggle( true );
  rotate->checked( true );

  MenuKit::Button::Ptr free = new MenuKit::Button();
  free->text( "Free" );
  free->enabled( false );
  //free->toggle( true );

  MenuKit::Button::Ptr scale = new MenuKit::Button();
  scale->text( "Scale" );
  scale->toggle( true );

  // organize
  top->append( file.get() );
  top->append( edit.get() );
  top->append( tools.get() );
  top->append( help.get() );

  file->append( open.get() );
  file->append( exit.get() );

  edit->append( move.get() );
  edit->append( spacer.get() );
  edit->append( cut.get() );
  edit->append( paste.get() );

  tools->append( search.get() );

  help->append( index.get() );
  help->append( about.get() );

  move->append( xyz.get() );
  move->append( rotate.get() );
  move->append( free.get() );
  move->append( scale.get() );

  return( top );
}
