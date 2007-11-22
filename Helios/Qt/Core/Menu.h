
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_HELIOS_QT_CORE_MENU_H__
#define __CADKIT_HELIOS_QT_CORE_MENU_H__

#include "MenuKit/Menu.h"

#include "Helios/Qt/Commands/BaseAction.h"

#include "QtGui/QMenu"

#include <set>

namespace CadKit {
namespace Helios {
namespace Core {

class Menu : public QMenu
{
  Q_OBJECT;
public:
  typedef QMenu BaseClass;
  typedef CadKit::Helios::Commands::BaseAction  BaseAction;
  typedef std::set<BaseAction::RefPtr>          Actions;

  /// Construction/Destruction.
  Menu ( const QString& title, QWidget* parent = 0x0 );
  virtual ~Menu();

  /// Get/Set the menu.
  void                        menu ( MenuKit::Menu* );
  MenuKit::Menu*              menu();
  const MenuKit::Menu*        menu() const;

protected slots:
  void _showMenu();
  void _hideMenu();

private:
  MenuKit::Menu::RefPtr _menu;
  Actions _actions;
};

}
}
}


#endif // __CADKIT_HELIOS_QT_CORE_MENU_H__
