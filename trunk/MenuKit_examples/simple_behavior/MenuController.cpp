#include "MenuController.h"

#include "osgProducer/EventAdapter"

MenuController::MenuController(osg::Group* g): BaseClass(),
  _behavior(new MenuKit::Behavior()),
  _mason(new MenuKit::OSG::osgVisualMason()),
  _attach(g),
  _kdm()
{
  typedef KeyboardDirectionMap KDM;
  typedef osgProducer::EventAdapter oPEA;
  typedef MenuKit::Behavior MKB;
  _kdm.insert( KDM::value_type(oPEA::KEY_Down,MKB::DOWN) );
  _kdm.insert( KDM::value_type(oPEA::KEY_Up,MKB::UP) );
  _kdm.insert( KDM::value_type(oPEA::KEY_Left,MKB::LEFT) );
  _kdm.insert( KDM::value_type(oPEA::KEY_Right,MKB::RIGHT) );
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

        // update the scene
        MenuKit::Menu* menu = _behavior->root();
        menu->accept( *_mason );

        /// TODO: find a more elegant way of doing this,
        /// TODO: possibly using the node's name
        /// TODO: THIS IS CAUSING A CRASH
        _attach->replaceChild(_attach->getChild(0),_mason->scene());
      }

      return true;
    } break;

  case osgGA::GUIEventAdapter::KEYUP:
    {
      return true;
    } break;

  default:
    break;
  }

  return false;
}
