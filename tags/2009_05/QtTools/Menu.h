
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

#include "QtTools/Export.h"
#include "QtTools/Action.h"

#include "MenuKit/Menu.h"

#include "QtGui/QMenu"

#include <set>

namespace QtTools {

class QT_TOOLS_EXPORT Menu : public QMenu
{
  Q_OBJECT;
public:

  // Typedefs.
  typedef QMenu BaseClass;
  typedef boost::shared_ptr<Menu> RefPtr;
  typedef std::set<Action::RefPtr> Actions;
  typedef std::set<Menu::RefPtr> Menus;

  /// Construction/Destruction.
  Menu ( const QString& title= "", QWidget* parent = 0x0 );
  virtual ~Menu();

  /// Get/Set the menu.
  void                        menu ( MenuKit::Menu* );
  MenuKit::Menu*              menu();
  const MenuKit::Menu*        menu() const;

protected slots:
  
  void                        _showMenu();
  
private:

  MenuKit::Menu::RefPtr _menu;
  Actions _actions;
  Menus _menus;
};

}


#endif // __CADKIT_HELIOS_QT_CORE_MENU_H__
