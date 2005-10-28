
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu button class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_MENU_BUTTON_CLASS_H_
#define _APP_FRAME_WORK_MENU_BUTTON_CLASS_H_

#include "AppFrameWork/Menus/MenuCommand.h"

#include "Usul/Pointers/Pointers.h"

#include <string>


namespace AFW {
namespace Menus {


class APP_FRAME_WORK_EXPORT MenuButton : public MenuCommand
{
public:

  // Typedefs.
  typedef MenuCommand BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MenuButton );

  // Constructors
  MenuButton();
  MenuButton ( const std::string &text, unsigned short underline = 0 );

  // Set/get the icon file.
  const std::string &                 icon() const { return _icon; }
  void                                icon ( const std::string &i ) { _icon = i; }

protected:

  // Use reference counting.
  virtual ~MenuButton();

private:

  // No copying.
  MenuButton ( const MenuButton & );
  MenuButton &operator = ( const MenuButton & );

  std::string _icon;
};


} // namespace Menus
} // namespace AFW


#endif //_APP_FRAME_WORK_MENU_BUTTON_CLASS_H_
