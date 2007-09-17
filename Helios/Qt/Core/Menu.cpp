
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Core/Menu.h"

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
//  Action for command.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  class Action : public CadKit::Helios::Commands::BaseAction
  {
  public:

    // Typedefs.
    typedef CadKit::Helios::Commands::BaseAction BaseClass;
    typedef Usul::Interfaces::IUnknown IUnknown;

    // Constructor.
    Action ( Usul::Commands::Command *command ) : BaseClass ( 0x0 ), _command ( command )
    {
      USUL_TRACE_SCOPE;

      if ( _command.valid () )
      {
        const std::string text      ( _command->text() );
        const std::string shortcut  ( _command->shortcut() );
        const std::string statusTip ( _command->statusTip() );
        const std::string toolTip   ( _command->toolTip() );
        const std::string iconPath  ( _command->iconPath() );

        if ( false == text.empty() )
          this->setText ( tr ( text.c_str() ) );

        if ( false == shortcut.empty() )
          this->setShortcut ( tr ( shortcut.c_str() ) );

        if ( false == statusTip.empty() )
          this->setStatusTip ( tr ( statusTip.c_str() ) );

        if ( false == toolTip.empty() )
          this->setToolTip ( tr ( toolTip.c_str() ) );

        if ( false == iconPath.empty() )
          CadKit::Helios::Tools::Image::icon ( command->iconPath().c_str(), this );
      }
    }

    // Destructor.
    virtual ~Action()
    {
      USUL_TRACE_SCOPE;
    }

  private:

    // No copying or assignment.
    Action ( const Action & );
    Action &operator = ( const Action & );

    virtual void _execute()
    {
      USUL_TRACE_SCOPE;
      USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "3551910933" );
      
      if ( _command.valid () )
      {
        Usul::Commands::Command::RefPtr command ( _command->clone () );
        command->execute ( 0x0 );
      }
    }

    Usul::Commands::Command::RefPtr _command;
  };
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
      _current ( menu ),
      _actions ( actions )
    {
    }

    virtual void apply ( MenuKit::Menu &m )
    {
      m.traverse ( *this );
    }

    virtual void apply ( MenuKit::Button &b )
    {
      if ( 0x0 != _current )
      {
        if ( b.separator () )
          _current->addSeparator ();
        else
        {
          Action::RefPtr action ( new Action ( b.command () ) );
          _current->addAction ( action.get () );
          action->setEnabled ( b.enabled () );
          _actions.insert ( action );
        }
      }
    }

  private:
    QMenu *_menu;
    QMenu *_current;
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
