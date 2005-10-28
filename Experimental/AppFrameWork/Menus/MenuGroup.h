
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu group class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_MENU_GROUP_CLASS_H_
#define _APP_FRAME_WORK_MENU_GROUP_CLASS_H_

#include "AppFrameWork/Menus/MenuItem.h"

#include "Usul/Pointers/Pointers.h"

#include <vector>
#include <string>


namespace AFW {
namespace Menus {


class APP_FRAME_WORK_EXPORT MenuGroup : public MenuItem
{
public:

  // Typedefs.
  typedef MenuItem BaseClass;
  typedef std::vector < MenuItem::ValidAccessRefPtr > Items;
  typedef Items::iterator Itr;
  typedef Items::const_iterator ConstItr;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MenuGroup );

  // Constructors
  MenuGroup();
  MenuGroup ( const std::string &text, unsigned short underline = 0 );

  // Append the item.
  void                                append ( MenuItem * );

  // Iterators.
  ConstItr                            begin() const { return _items.begin(); }
  Itr                                 begin()       { return _items.begin(); }

  // Iterators.
  ConstItr                            end() const { return _items.end(); }
  Itr                                 end()       { return _items.end(); }

protected:

  // Use reference counting.
  virtual ~MenuGroup();

private:

  // No copying.
  MenuGroup ( const MenuGroup & );
  MenuGroup &operator = ( const MenuGroup & );

  Items _items;
};


} // namespace Menus
} // namespace AFW


#endif //_APP_FRAME_WORK_MENU_GROUP_CLASS_H_
