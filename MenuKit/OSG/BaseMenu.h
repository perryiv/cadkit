
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base menu class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_OSG_BASE_MENU_CLASS_H_
#define _MENUKIT_OSG_BASE_MENU_CLASS_H_

#include "MenuKit/OSG/Skin.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/Behavior.h"

#include "osg/MatrixTransform"


namespace MenuKit {
namespace OSG {


class MENUKIT_OSG_EXPORT BaseMenu : public MenuKit::Referenced
{
public:

  // Useful typedefs
  MENUKIT_DECLARE_POINTER ( BaseMenu );
  typedef MenuKit::Referenced  BaseClass;
  typedef osg::MatrixTransform MT;
  typedef MenuKit::OSG::Skin Skin;
  typedef MenuKit::Behavior Behavior;

  // Set/get the menu.
  const MenuKit::Menu *   menu() const;
  MenuKit::Menu *         menu();
  void                    menu ( MenuKit::Menu * );

  // Set/get the skin.
  const Skin *            skin() const;                 // get skin pointer
  Skin *                  skin();                       // get skin pointer
  void                    skin ( Skin * );              // set skin pointer

  // Set/get the behavior.
  const Behavior *        behavior() const;
  Behavior *              behavior();
  void                    behavior ( Behavior * );

  // Set/get the behavior.
  const MT *              scene() const;
  MT *                    scene();
  void                    scene ( MT * );

protected:

  BaseMenu();
  virtual ~BaseMenu();

private:

  typedef MenuKit::Item::Ptr    ItemPtr;
  typedef MenuKit::Button::Ptr  ButtonPtr;
  typedef MenuKit::Menu::Ptr    MenuPtr;
  typedef Behavior::Ptr         BehaviorPtr;
  typedef Skin::Ptr             SkinPtr;
  typedef osg::ref_ptr<MT>      TransformPtr;

  MenuPtr       _menu;
  SkinPtr       _skin;
  BehaviorPtr   _behavior;
  TransformPtr  _scene;
};


}; // namespace OSG
}; // namespace MenuKit


#endif // _MENUKIT_OSG_BASE_MENU_CLASS_H_
