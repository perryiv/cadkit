#include "MenuController.h"

#include "MenuKit/OSG/VisualThemeSkin.h"

#include "osgProducer/EventAdapter"
#include "osgText/Font"

MenuController::MenuController(osg::Group* g,const std::string& f): BaseClass(),
  _behavior(new MenuKit::Behavior()),
  _mason(new MenuKit::OSG::osgMason()),
  _attach(g),
  _kdm(),
  _fontfile(f)
{
  // --- map the keyboard commands to the behavior class --- //
  typedef KeyboardDirectionMap KDM;
  typedef osgProducer::EventAdapter oPEA;
  typedef MenuKit::Behavior MKB;
  _kdm.insert( KDM::value_type(oPEA::KEY_Down,MKB::DOWN) );
  _kdm.insert( KDM::value_type(oPEA::KEY_Up,MKB::UP) );
  _kdm.insert( KDM::value_type(oPEA::KEY_Left,MKB::LEFT) );
  _kdm.insert( KDM::value_type(oPEA::KEY_Right,MKB::RIGHT) );


  // --- make the graphics objects --- //

  // load a skin
  osg::ref_ptr<osgText::Font> font = osgText::readFontFile( _fontfile );

  // make a skin
  MenuKit::OSG::VisualThemeSkin::Ptr skin = new MenuKit::OSG::VisualThemeSkin();
  if( font.valid() )
    skin->font( font.get() );

  MenuKit::OSG::osg_color_theme ct;
  MenuKit::OSG::osg_color_map norm=ct.get_map();
  MenuKit::OSG::osg_color_map hi=ct.get_map();
  osg::Vec4 lightblue(0.5,0.5,1.0,1.0);
  hi["background"] = lightblue;
  MenuKit::OSG::osg_color_map dis=ct.get_map();
  osg::Vec4 darkgrey(0.5,0.5,0.5,1.0);
  dis["text"] = darkgrey;

  typedef MenuKit::OSG::osgThemeSkinTile::DisplayModeThemeMap DMTMAP;
  DMTMAP dmtmap;
  dmtmap.insert( DMTMAP::value_type(MenuKit::OSG::TileFunctor::NORMAL,norm) );
  dmtmap.insert( DMTMAP::value_type(MenuKit::OSG::TileFunctor::HIGHLIGHT,hi) );
  dmtmap.insert( DMTMAP::value_type(MenuKit::OSG::TileFunctor::DISABLED,dis) );

  MenuKit::OSG::osgThemeSkinTile::Ptr tile = new MenuKit::OSG::osgThemeSkinTile();
  tile->theme_map( dmtmap );
  tile->skin( skin.get() );

  _mason->tile( tile.get() );
}

MenuController::~MenuController()
{
}

bool MenuController::handle(const osgGA::GUIEventAdapter& ea,
                            osgGA::GUIActionAdapter &us)
{
  switch( ea.getEventType() )
  {
  case osgGA::GUIEventAdapter::KEYDOWN:
    {
      KeyboardDirectionMap::iterator iter = _kdm.find(ea.getKey());
      if( _kdm.end() != iter )
      {
        // use the behavior class to modify the menu
        _behavior->moveFocused( iter->second );
 
        // build a new scene
        MenuKit::Menu* menu = _behavior->root();
        menu->accept( *_mason );

        // FIND the child in the tree with the same name as the node that the mason produced
        const std::string& menuname( _mason->scene_name() );
        unsigned int index(0);
        unsigned int maxchild(_attach->getNumChildren());
        for(; index<maxchild; index++)
        {
          const std::string& childname(_attach->getChild(index)->getName());
          if( childname == menuname )
            break;
        }

        //if( index != maxchild )
        //{
        //  _attach->removeChild(index,1);
        //  _attach->addChild(_mason->scene());
        //}
        //else
        //  _attach->addChild(_mason->scene());
        unsigned int num(_attach->getNumChildren());
        if( num )
          _attach->removeChild(0,num);
        _attach->addChild( _mason->scene() );
      }

      return false;
    } break;

  case osgGA::GUIEventAdapter::KEYUP:
    {
      return false;
    } break;

  default:
    break;
  }

  return false;
}
