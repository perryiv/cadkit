
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu-bar window class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_MENU_BAR_CLASS_H_
#define _APP_FRAME_WORK_MENU_BAR_CLASS_H_

#include "AppFrameWork/Windows/Group.h"

namespace AFW { namespace Menus { class MenuGroup; } }


namespace AFW {
namespace Menus {


  class APP_FRAME_WORK_EXPORT MenuBar : public AFW::Windows::Group
{
public:

  // Typedefs.
  typedef AFW::Windows::Group BaseClass;

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

  // Get or (if asked to) make the menu.
  MenuGroup *                         menu ( const std::string &text, bool createIfNeeded = true );

  // Build a default GUI.
  virtual void                        init();

protected:

  // Constructor
  MenuBar();

  // Use reference counting.
  virtual ~MenuBar();

  void                                _buildDefault();

private:

  // No copying.
  MenuBar ( const MenuBar & );
  MenuBar &operator = ( const MenuBar & );

  AFW_DECLARE_OBJECT ( MenuBar );
};


} // namespace Menus
} // namespace AFW


#endif //_APP_FRAME_WORK_MENU_BAR_CLASS_H_
