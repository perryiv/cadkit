#include "MenuController.h"

#include "osgProducer/EventAdapter"

MenuController::MenuController(/*osg::Group* g*/): BaseClass(),
  _attach(0x0/*g*/),
  _mason(0x0/*new TileMason()*/),
  _behavior(0x0/*new MenuKit::Behavior()*/),
  _kdm()
{
  // --- map the keyboard commands to the behavior class --- //
  typedef KeyboardDirectionMap KDM;
  typedef osgProducer::EventAdapter oPEA;
  typedef MenuKit::Behavior MKB;
  _kdm.insert( KDM::value_type(oPEA::KEY_Down,MKB::DOWN) );
  _kdm.insert( KDM::value_type(oPEA::KEY_Up,MKB::UP) );
  _kdm.insert( KDM::value_type(oPEA::KEY_Left,MKB::LEFT) );
  _kdm.insert( KDM::value_type(oPEA::KEY_Right,MKB::RIGHT) );

  //--- allocate a skin ---//
  //typedef MenuKit::OSG::VisualThemeSkin aSkin;
  //aSkin::Ptr skin = new aSkin();
  //skin->letter_height( 50.0f );

  //--- setup the skin ---//
  // load font for skin
  //std::string fontfile("");
  //if( argc > 1 && osgDB::fileExists(argv[1]) )
  //  fontfile = argv[1];
  //else
  //  fontfile = "C:\\sdk\\share\\OpenSceneGraph-Data\\fonts\\arial.ttf";
  //osg::ref_ptr<osgText::Font> font = osgText::readFontFile( fontfile );
  //if( font.valid() )
  //  skin->font( font.get() );
  //--- --------------- ---//

  //--- allocate a tile ---//
  //typedef MenuKit::OSG::SkinTile<aSkin::base_class> aTile;
  //aTile::Ptr tile = new aTile();

  //--- setup the tile ---//
  //tile->skin( skin.get() );  // no more skin code after here
  //--- --------------- ---//

  // make a mason
  //_mason->tile( tile.get() );
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

        if( index != maxchild )
        {
          _attach->removeChild(index,1);
          _attach->addChild(_mason->scene());
        }
        else
          _attach->addChild(_mason->scene());
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
