
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

#include "AppFrameWork/Core/Group.h"


namespace AFW {
namespace Menus {


class APP_FRAME_WORK_EXPORT MenuBar : public AFW::Core::Group
{
public:

  // Typedefs.
  typedef AFW::Core::Group BaseClass;

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

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
