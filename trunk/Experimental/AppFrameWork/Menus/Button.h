
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all menu commands.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_MENU_BUTTON_CLASS_H_
#define _APP_FRAME_WORK_MENU_BUTTON_CLASS_H_

#include "AppFrameWork/Core/Window.h"


namespace AFW {
namespace Menus {


class APP_FRAME_WORK_EXPORT Button : public AFW::Core::Window
{
public:

  // Typedefs.
  typedef AFW::Core::Window BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Button );

  // Constructor
  Button ( const std::string &text = "", AFW::Core::Icon *icon = 0x0, unsigned short underline = 0xFFFF );

  // Set/get the character to underline.
  unsigned short                      underline() const;
  void                                underline ( unsigned short );

protected:

  // Use reference counting.
  virtual ~Button();

private:

  // No copying.
  Button ( const Button & );
  Button &operator = ( const Button & );

  unsigned short _underline;
};


} // namespace Menus
} // namespace AFW


#endif //_APP_FRAME_WORK_MENU_BUTTON_CLASS_H_
