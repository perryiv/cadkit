
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The menu class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_OSG_MENU_CLASS_H_
#define _MENUKIT_OSG_MENU_CLASS_H_

#include "MenuKit/OSG/BaseMenu.h"


namespace MenuKit {
namespace OSG {


class MENUKIT_OSG_EXPORT Menu : public MenuKit::OSG::BaseMenu
{
public:

  // Useful typedefs
  USUL_DECLARE_REF_POINTERS ( Menu );
  typedef MenuKit::OSG::BaseMenu  BaseClass;

  // Constructor.
  Menu();

  // Interaction.
  void                    toggleVisible();
  void                    selectFocused();
  void                    moveFocused ( MenuKit::Behavior::Direction d );

  // Place the menu using the given matrix.
  void                    place ( const osg::Matrix &m );

  // Update the menu.
  void                    updateMenu();

  // Update the scene.
  void                    updateScene();

  /// Is the menu visible?
  bool                    isVisible () const;

protected:

  virtual ~Menu();

  // Remove all children from the group.
  static void             _removeAllChildren ( osg::Group * );
};


}; // namespace OSG
}; // namespace MenuKit


#endif // _MENUKIT_OSG_MENU_CLASS_H_
