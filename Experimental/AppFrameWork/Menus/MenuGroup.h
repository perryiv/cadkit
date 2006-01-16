
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

#include "AppFrameWork/Windows/Group.h"


namespace AFW {
namespace Menus {


class APP_FRAME_WORK_EXPORT MenuGroup : public AFW::Windows::Group
{
public:

  // Typedefs.
  typedef AFW::Windows::Group BaseClass;

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

  // Set/get the character to underline.
  unsigned short                      underline() const;
  void                                underline ( unsigned short );

protected:

  // Constructor
  MenuGroup();

  // Use reference counting.
  virtual ~MenuGroup();

private:

  // No copying.
  MenuGroup ( const MenuGroup & );
  MenuGroup &operator = ( const MenuGroup & );

  unsigned short _underline;

  AFW_DECLARE_OBJECT ( MenuGroup );
};


} // namespace Menus
} // namespace AFW


#endif //_APP_FRAME_WORK_MENU_GROUP_CLASS_H_
