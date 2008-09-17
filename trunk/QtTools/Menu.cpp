
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "QtTools/Menu.h"
#include "QtTools/Action.h"

#include "QtTools/Image.h"

#include "MenuKit/Visitor.h"
#include "MenuKit/UpdateVisitor.h"
#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/RadioButton.h"
#include "MenuKit/Separator.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Threads/Named.h"

using namespace QtTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Menu::Menu ( const QString& title, QWidget* parent ) :
BaseClass ( title, parent ),
_menu ( 0x0 ),
_actions()
{
  USUL_TRACE_SCOPE;

  // Set the context-menu policy.
  this->setContextMenuPolicy ( Qt::CustomContextMenu );

  // Set up slots.
  connect ( this, SIGNAL ( aboutToShow() ), SLOT ( _showMenu() ) );
  connect ( this, SIGNAL ( customContextMenuRequested ( const QPoint & ) ), SLOT ( _showContextMenu ( const QPoint & ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Menu::~Menu()
{
  USUL_TRACE_SCOPE;
  this->clear();
  _actions.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the menu.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::menu ( MenuKit::Menu* menu )
{
  USUL_TRACE_SCOPE;
  _menu = menu;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu.
//
///////////////////////////////////////////////////////////////////////////////

MenuKit::Menu* Menu::menu()
{
  USUL_TRACE_SCOPE;
  return _menu.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu.
//
///////////////////////////////////////////////////////////////////////////////

const MenuKit::Menu* Menu::menu() const
{
  USUL_TRACE_SCOPE;
  return _menu.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail 
{

  class QtMenuBuilder : public MenuKit::Visitor
  {
  public:
    typedef MenuKit::Visitor  BaseClass;
    typedef QtTools::Action   Action;

    QtMenuBuilder ( QMenu* menu, Menu::Actions &actions ) : BaseClass(),
      _menu ( menu ),
      _actions ( actions )
    {
      USUL_TRACE_SCOPE;
    }

    virtual void apply ( MenuKit::Menu &m )
    {
      USUL_TRACE_SCOPE;

      Menu * menu ( new Menu ( m.text().c_str() ) );
      menu->menu ( &m ); 
      
      if ( 0x0 != _menu )
        _menu->addMenu ( menu );
    }

    virtual void apply ( MenuKit::Button &b )
    {
      USUL_TRACE_SCOPE;

      Action::RefPtr action ( new Action ( b.command() ) );
      
      action->setEnabled ( b.enabled() );
      _actions.insert ( action );

      if ( 0x0 != _menu )
        _menu->addAction ( action.get() );
    }

    virtual void apply ( MenuKit::ToggleButton &toggle )
    {
      USUL_TRACE_SCOPE;

      Action::RefPtr action ( new Action ( toggle.command() ) );
      
      action->setEnabled ( toggle.enabled() );
      _actions.insert ( action );

      action->setCheckable ( true );
      action->setChecked ( toggle.checked() );

      if ( 0x0 != _menu )
        _menu->addAction ( action.get() );
    }

    virtual void apply ( MenuKit::RadioButton &radio )
    {
      USUL_TRACE_SCOPE;

      Action::RefPtr action ( new Action ( radio.command() ) );
      
      action->setEnabled ( radio.enabled() );
      _actions.insert ( action );

      action->setCheckable ( true );
      action->setChecked ( radio.checked() );

      if ( 0x0 != _menu )
        _menu->addAction ( action.get() );
    }

    virtual void apply ( MenuKit::Separator &s )
    {
      USUL_TRACE_SCOPE;

      if ( 0x0 != _menu )
      {
        _menu->addSeparator();
      }
    }

  private:
    QMenu *_menu;
    Menu::Actions &_actions;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Menu is about to be shown.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::_showMenu()
{
  USUL_TRACE_SCOPE;

  // Clear what we may have.
  this->clear();
  _actions.clear();
  
  if ( _menu.valid() )
  {
    // Update internal state.
    MenuKit::UpdateVisitor::RefPtr update ( new MenuKit::UpdateVisitor );
    _menu->accept ( *update );

    // Build the qt menu.
    MenuKit::Visitor::RefPtr visitor ( new Detail::QtMenuBuilder ( this, _actions ) );
    _menu->traverse ( *visitor );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the mouse is pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::mousePressEvent ( QMouseEvent * event )
{
  USUL_TRACE_SCOPE;
	BaseClass::mousePressEvent ( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the mouse is released.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::mouseReleaseEvent ( QMouseEvent * event )
{
  USUL_TRACE_SCOPE;
	BaseClass::mouseReleaseEvent ( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the context menu is requested.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::_showContextMenu ( const QPoint & )
{
  USUL_TRACE_SCOPE;
}
