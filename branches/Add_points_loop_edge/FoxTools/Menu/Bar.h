
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu-bar class for menus.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_MENU_BAR_CLASS_H_
#define _FOX_TOOLS_MENU_BAR_CLASS_H_

#include "FoxTools/Menu/Group.h"

#include <vector>

namespace FX { class FXObject; class FXComposite; class FXMenuBar; class FXMDIClient; class FXMDIMenu; }


namespace FoxTools {
namespace Menu {


class FOX_TOOLS_EXPORT Bar : public FoxTools::Items::Item
{
public:

  // Typedefs.
  typedef FoxTools::Items::Item BaseClass;
  typedef std::vector<Group::ValidRefPtr> Groups;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Bar );

  // Constructor.
  Bar ( FX::FXComposite *dockedSite, FX::FXComposite *undockedSite );

  // Append the group.
  void                  append ( Group * );

  // Build the menus.
  virtual void          build();

  // Clear the menus.
  virtual void          clear();

  // Set the client area
  void                  clientArea ( FX::FXMDIClient* clientArea ) { _clientArea = clientArea; }

  // Create the windows.
  virtual void          create();

  // Is the menu bar empty?
  bool                  empty() const;

  // Find the child that made the given object.
  Item *                find ( FX::FXObject * );

  // Get group with given name.  Return null if doesn't exist.
  Group *               group ( const std::string& name );

  // Insert group before given group name.
  void                  insert ( const std::string&, Group * );

  // Set the mdi ment
  void                  mdiMenu ( FX::FXMDIMenu *mdiMenu ) { _mdiMenu = mdiMenu; }

  // Purge all commands with given token.
  virtual void          purge ( unsigned int token );

  // Return the size of the menu bar.
  unsigned int          size() const;

protected:

  // Use reference counting.
  virtual ~Bar();

  void                  _deleteMenuBar();

private:

  // No copying.
  Bar ( const Bar & );
  Bar &operator = ( const Bar & );

  Groups _groups;
  FX::FXMenuBar *_bar;
  FX::FXComposite *_dockedSite;
  FX::FXComposite *_undockedSite;

  FX::FXMDIClient * _clientArea;
  FX::FXMDIMenu *   _mdiMenu;
};


}; // namespace Menu
}; // namespace FoxTools


#endif // _FOX_TOOLS_MENU_BAR_CLASS_H_
