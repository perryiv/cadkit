
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_GUI_MENU_CLASS_H_
#define _VRV_GUI_MENU_CLASS_H_

#include "VRV/GUI/CompileGuard.h"

#include "VRV/Interfaces/IMenuRead.h"
#include "VRV/Interfaces/IMenuGet.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"

#include "MenuKit/Menu.h"


namespace VRV {
namespace GUI {


class Menu : public Usul::Base::Referenced,
             public VRV::Interfaces::IMenuRead,
             public VRV::Interfaces::IMenuGet,
             public Usul::Interfaces::IPlugin
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Menu );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction.
  Menu();

protected:

  // Do not copy.
  Menu ( const Menu & );
  Menu &operator = ( const Menu & );

  // Use reference counting.
  virtual ~Menu();

  /// Usul::Interfaces::IPlugin.
  virtual std::string getPluginName() const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  VRV::Interfaces::IMenuRead
  //
  /////////////////////////////////////////////////////////////////////////////

  // Read the file and build the menu.
  virtual void                    readMenuFile ( const std::string &filename, Unknown *caller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  VRV::Interfaces::IMenuGet
  //
  /////////////////////////////////////////////////////////////////////////////

  // Return a pointer to the menu.
  virtual const MenuKit::Menu *   getMenu() const;
  virtual MenuKit::Menu *         getMenu();

  /////////////////////////////////////////////////////////////////////////////

private:

  MenuKit::Menu::Ptr _menu;
};


}; // namespace GUI
}; // namespace VRV


#endif // _VRV_GUI_MENU_CLASS_H_
