
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
    fontfile = "C:\\sdk\\share\\OpenSceneGraph-Data\\fonts\\arial.ttf";

  osg::ref_ptr<osgText::Font> font = osgText::readFontFile( fontfile );

  // define some themes
  osg::Vec4 red(1.0,0.0,0.0,1.0);
  osg::Vec4 green(0.0,1.0,0.0,1.0);
  MenuKit::OSG::osg_color_theme ct;
  MenuKit::OSG::osg_color_map hi(ct.get_map()),
                              norm(ct.get_map()),
                              dis(ct.get_map());
  hi["text"] = red;
  dis["text"] = green;

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

  // set the size for the tile
  tile->box( Detail::Box(tile->height(*button), tile->width(*button)) );
  //skin->graphic_width( skin->width(*button) );
  MenuKit::OSG::osgThemeSkinTile::DisplayModeThemeMap::const_iterator iter;
  iter = dmtmap.find( MenuKit::OSG::TileFunctor::DISABLED );
  if( iter != dmtmap.end() )
    tile->mode( iter->first );
    //skin->theme( iter->second );
  else
    tile->mode( iter->first );
    //skin->theme( norm );

  // generate the graphic for the item
  osg::ref_ptr<osg::Node> scene = tile->operator ()( *button );
  //osg::ref_ptr<osg::Node> scene = skin->operator ()(*button);

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
