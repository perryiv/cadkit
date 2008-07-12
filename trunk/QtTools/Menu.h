
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
  typedef QMenu BaseClass;
  typedef std::set<Action::RefPtr> Actions;

  /// Construction/Destruction.
  Menu ( const QString& title= "", QWidget* parent = 0x0 );
  virtual ~Menu();

  /// Get/Set the menu.
  void                        menu ( MenuKit::Menu* );
  MenuKit::Menu*              menu();
  const MenuKit::Menu*        menu() const;

  /// Overload these mouse events.
  virtual void                mousePressEvent ( QMouseEvent * event );
	virtual void                mouseReleaseEvent ( QMouseEvent * event );

protected slots:
  
  void                        _showContextMenu ( const QPoint & );
  void                        _showMenu();
  
private:
  MenuKit::Menu::RefPtr _menu;
  Actions _actions;
};

}


#endif // __CADKIT_HELIOS_QT_CORE_MENU_H__
