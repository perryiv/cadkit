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
#include "MenuKit/OSG/osg_types.h"

#include <map>
#include <string>

/** MenuController
  * MenuController is a osgGA::GUIEventHandler that
  * maps the MenuKit::Behavior class to keyboard presses
  * caught by the osgGA library
  */
class MenuController : public osgGA::GUIEventHandler
{
public:
  typedef osgGA::GUIEventHandler BaseClass;
  typedef std::map<int,MenuKit::Behavior::Direction> KeyboardDirectionMap;

  MenuController(osg::Group* g, const std::string& fontfile);

  /// takes user input and possibly modifies the scene
  virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);

  virtual void accept(osgGA::GUIEventHandlerVisitor& v) { v.visit(*this); }

  void behavior(MenuKit::Behavior* b) { _behavior = b; }
  const MenuKit::Behavior* behavior() const { return _behavior.get(); }
  MenuKit::Behavior* behavior() { return _behavior.get(); }

  void mason(MenuKit::OSG::osgMason* m) { _mason = m; }
  const MenuKit::OSG::osgMason* mason() const { return _mason.get(); }
  MenuKit::OSG::osgMason* mason() { return _mason.get(); }

  void attach_node(osg::Group* v) { _attach = v; }
  const osg::Group* attach_node() const { return _attach.get(); }

  void keyboard_direction_map(const KeyboardDirectionMap& m) { _kdm = m; }
  const KeyboardDirectionMap& keyboard_direction_map() const { return _kdm; }

  void font_file(const std::string& f) { _fontfile = f; }
  const std::string& font_file() const { return _fontfile; }

protected:
  virtual ~MenuController();

private:
  MenuController();   // not implemented by design

  MenuKit::Behavior::Ptr _behavior;
  MenuKit::OSG::osgMason::Ptr _mason;
  osg::ref_ptr<osg::Group> _attach;  // can not be a ref_ptr from circular reference problem
  KeyboardDirectionMap _kdm;
  std::string _fontfile;
};

#endif
