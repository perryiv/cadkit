
//#ifdef _WIN32
//# pragma warning ( disable : 4541 ) // warning about multiple inheritance within osg
//#endif

#include "osg/MatrixTransform"
#include "osgProducer/Viewer"
#include "osgDB/FileUtils"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/OSG/osg_types.h"
#include "MenuKit/OSG/VisualThemeSkin.h"

int main(int argc, char* argv[])
{
  // load font for skin
  std::string fontfile("");
  if( argc > 1 && osgDB::fileExists(argv[1]) )
    fontfile = argv[1];
  else
    fontfile = "C:\\sdk\\share\\OpenSceneGraph-Data\\fonts\\dirtydoz.ttf";

  osg::ref_ptr<osgText::Font> font = osgText::readFontFile( fontfile );

  // define some themes
  MenuKit::OSG::osg_color_theme ct;
  MenuKit::OSG::osg_color_map hi(ct.get_map()),
                              norm(ct.get_map()),
                              dis(ct.get_map());

  // allocate & setup a skin
  MenuKit::OSG::VisualThemeSkin::Ptr skin = new MenuKit::OSG::VisualThemeSkin();
  if( font.valid() )
    skin->font( font.get() );

  // allocate a tile
  MenuKit::OSG::osgThemeSkinTile::Ptr tile = new MenuKit::OSG::osgThemeSkinTile();

  // set up the tile
  // these setup steps are extremely important
  // the keys inserted will be used by a Mason is traversing a menu
  tile->skin( skin.get() );

  typedef MenuKit::OSG::osgThemeSkinTile::DisplayModeThemeMap DMTMAP;
  DMTMAP dmtmap;
  dmtmap.insert( DMTMAP::value_type(MenuKit::OSG::TileFunctor::NORMAL,norm) );
  dmtmap.insert( DMTMAP::value_type(MenuKit::OSG::TileFunctor::HIGHLIGHT,hi) );
  dmtmap.insert( DMTMAP::value_type(MenuKit::OSG::TileFunctor::DISABLED,dis) );
  tile->theme_map( dmtmap );

  // define a menu item
  MenuKit::Menu::Ptr menu = new MenuKit::Menu("File","",MenuKit::Menu::VERTICAL);
  MenuKit::Menu::Ptr button = new MenuKit::Menu("Open","",MenuKit::Menu::VERTICAL);
  //MenuKit::Button::Ptr button = new MenuKit::Button("Open","");
  //button->expanded( true );
  //button->toggle( true );
  //button->radio( true );
  //button->checked( true );
  menu->append( button.get() );

  // set the size for the tile
  MenuKit::Detail::Box box(tile->height(*button),tile->width(*button));
  tile->box( box );

  // generate the graphic for the item
  osg::ref_ptr<osg::Node> scene = tile->operator ()( *button );

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
