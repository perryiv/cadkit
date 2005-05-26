
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Group class for menus.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_MENU_GROUP_CLASS_H_
#define _FOX_TOOLS_MENU_GROUP_CLASS_H_

#include "FoxTools/Menu/Item.h"

#include <vector>

namespace FX { class FXObject; class FXWindow; class FXComposite; class FXMenuPane; class FXMenuCaption; }


namespace FoxTools {
namespace Menu {

class Bar;

class FOX_TOOLS_EXPORT Group : public FoxTools::Menu::Item
{
public:

  // Typedefs.
  typedef FoxTools::Menu::Item BaseClass;
  typedef std::vector<BaseClass::ValidRefPtr> Items;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Group );

  // Constructors.
  Group();
  Group ( const std::string & );

  // Append the item.
  void                  append ( Item * );

  // Clear the menu.
  virtual void          clear();

  // Create the windows if we can.
  void                  create();

  // Is the group empty?
  bool                  empty() const;

  // Find the child that made the given object.
  Item *                find ( FX::FXObject * );

  // Purge all commands with given token.
  virtual void          purge ( unsigned int token );

  // Return the size of the group.
  unsigned int          size() const;

  // Set/get the scroll flag.
  void                  scroll ( bool );
  bool                  scroll() const;

protected:

  // Use reference counting.
  virtual ~Group();

  void                  _build ( FX::FXWindow *paneParent, FX::FXComposite *captionParent );

  void                  _deleteWindows();

private:

  // No copying.
  Group ( const Group & );
  Group &operator = ( const Group & );

  friend class Bar;

  Items _items;
  FX::FXMenuPane *_pane;
  FX::FXMenuCaption *_caption;
  bool _scroll;
};


}; // namespace Menu
}; // namespace FoxTools


#endif // _FOX_TOOLS_MENU_GROUP_CLASS_H_
