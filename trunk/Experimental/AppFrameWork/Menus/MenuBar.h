
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

#include "AppFrameWork/Menus/MenuGroup.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <vector>

namespace AFW { namespace Core { class MainWindow; } }


namespace AFW {
namespace Menus {


class APP_FRAME_WORK_EXPORT MenuBar : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef std::vector < MenuGroup::ValidAccessRefPtr > Menus;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MenuBar );

  // Possible flags
  enum
  {
    DIRTY = 0x01
  };

  // Constructor
  MenuBar();

  // Build a default GUI.
  void                                buildDefault();

  // Set/get the dirty flag.
  void                                dirty ( bool );
  bool                                dirty() const;

  // Access to the items.
  const Menus &                       menus() const { return _menus; }
  Menus &                             menus()       { return _menus; }

protected:

  // Use reference counting.
  virtual ~MenuBar();

  void                                _buildDefault();

private:

  // No copying.
  MenuBar ( const MenuBar & );
  MenuBar &operator = ( const MenuBar & );

  Menus _menus;
  unsigned int _flags;
  AFW::Core::MainWindow *_parent;
};


} // namespace Menus
} // namespace AFW


#endif //_APP_FRAME_WORK_MENU_BAR_CLASS_H_
