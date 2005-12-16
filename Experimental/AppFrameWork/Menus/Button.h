
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

  // Menu button types.
  enum Type
  {
    MENU_BUTTON,
    MENU_RADIO,
    MENU_CHECK,
    MENU_SEPARATOR
  };

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

  // Factory functions.
  static Button *                     createButton ( const std::string &text, const Type &type, const AFW::Core::Icon &icon );
  static Button *                     createButton ( const std::string &text, const AFW::Core::Icon &icon );
  static Button *                     createButton ( const std::string &text, const std::string &icon );
  static Button *                     createButton ( const std::string &text );
  static Button *                     createCheck  ( const std::string &text );
  static Button *                     createRadio  ( const std::string &text );
  static Button *                     createSeparator();
                                               
  // Set/get the character to underline.
  unsigned short                      underline() const;
  void                                underline ( unsigned short );

  // Set/get the type.
  void                                type ( const Type & );
  Type                                type() const;

protected:

  // Constructor
  Button();

  // Use reference counting.
  virtual ~Button();

private:

  // No copying.
  Button ( const Button & );
  Button &operator = ( const Button & );

  unsigned short _underline;
  Type _type;

  AFW_DECLARE_OBJECT ( Button );
};


} // namespace Menus
} // namespace AFW


#endif //_APP_FRAME_WORK_MENU_BUTTON_CLASS_H_
