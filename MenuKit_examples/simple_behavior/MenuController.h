#ifndef _menucontroller_h_
#define _menucontroller_h_
/* MenuController
 * A class that uses keyboard/mouse input (from osgGA.lib)
 * to interact with MenuKit's Behavior class
 * which configures the menu that will be displayed.
 * It also updates a node that is used in the scene.
 */

#include "osgGA/GUIEventHandler"
#include "osgGA/GUIEventAdapter"
#include "osgGA/GUIActionAdapter"

#include "osgProducer/OsgCameraGroup"
#include "MenuKit/Behavior.h"
#include "MenuKit/OSG/TileMason.h"
#include <map>

/** MenuController
  * Marries the behavior object to keyboard presses
  */
class MenuController : public osgGA::GUIEventHandler
{
public:
  typedef osgGA::GUIEventHandler BaseClass;
  typedef std::map<int,MenuKit::Behavior::Direction> KeyboardDirectionMap;

  MenuController(osg::Group* g);

  /// takes user input and possibly modifies the scene
  virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);

  virtual void accept(osgGA::GUIEventHandlerVisitor& v) { v.visit(*this); }

  void behavior(MenuKit::Behavior* b) { _behavior = b; }
  const MenuKit::Behavior* behavior() const { return _behavior.get(); }
  MenuKit::Behavior* behavior() { return _behavior.get(); }

  void mason(MenuKit::OSG::osgVisualMason* m) { _mason = m; }
  const MenuKit::OSG::osgVisualMason* mason() const { return _mason.get(); }
  MenuKit::OSG::osgVisualMason* mason() { return _mason.get(); }

  void attach_node(osg::Group* v) { _attach = v; }
  const osg::Group* attach_node() const { return _attach; }

  void keyboard_direction_map(const KeyboardDirectionMap& m) { _kdm = m; }
  const KeyboardDirectionMap& keyboard_direction_map() const { return _kdm; }

protected:
  virtual ~MenuController();

private:
  MenuController();   // not implemented by design

  MenuKit::Behavior::Ptr _behavior;
  MenuKit::OSG::osgVisualMason::Ptr _mason;
  osg::Group* _attach;  // can not be a ref_ptr from circular reference problem
  KeyboardDirectionMap _kdm;
};

#endif
