
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Core/Menu.h"
#include "Helios/Qt/Commands/Action.h"
#include "Helios/Qt/Tools/Image.h"

#include "MenuKit/Visitor.h"
#include "MenuKit/UpdateVisitor.h"
#include "MenuKit/Button.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Threads/Named.h"

using namespace CadKit::Helios::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Menu::Menu ( const QString& title, QWidget* parent ) :
BaseClass ( title, parent ),
_menu ( 0x0 ),
_actions ()
{
  connect ( this, SIGNAL ( aboutToShow() ), SLOT ( _showMenu () ) );
  connect ( this, SIGNAL ( aboutToHide() ), SLOT ( _hideMenu () ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Menu::~Menu ()
{
  _actions.clear ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the menu.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::menu ( MenuKit::Menu* menu )
{
  _menu = menu;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu.
//
///////////////////////////////////////////////////////////////////////////////

MenuKit::Menu* Menu::menu ()
{
  return _menu.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu.
//
///////////////////////////////////////////////////////////////////////////////

const MenuKit::Menu* Menu::menu () const
{
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
    typedef MenuKit::Visitor BaseClass;

    QtMenuBuilder ( QMenu* menu, Menu::Actions &actions ) : BaseClass (),
      _menu ( menu ),
      _actions ( actions )
    {
    }

    virtual void apply ( MenuKit::Menu &m )
    {
      // Save the old menu.
      QMenu* current ( _menu );

      // Add a new menu.
      if ( 0x0 != _menu )
        _menu = _menu->addMenu ( QObject::tr ( m.name().c_str() ) );

      // Traverse the menu.
      m.traverse ( *this );

      // Restore the current menu.
      _menu = current;
    }

    virtual void apply ( MenuKit::Button &b )
    {
      if ( 0x0 != _menu )
      {
        if ( b.separator () )
          _menu->addSeparator ();
        else
        {
          typedef CadKit::Helios::Commands::Action Action;
          Action::RefPtr action ( new Action ( b.command () ) );

          // See if it's a toggle.
          if ( b.toggle () )
          {
            action->setCheckable ( true );
            action->setChecked ( b.checked () );
          }

          _menu->addAction ( action.get () );
          action->setEnabled ( b.enabled () );
          _actions.insert ( action );
        }
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

void Menu::_showMenu ()
{
  // Clear what we may have.
  this->clear ();
  _actions.clear();
  
  if ( _menu.valid () )
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
//  Menu is about to be hidden.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::_hideMenu ()
{
}
