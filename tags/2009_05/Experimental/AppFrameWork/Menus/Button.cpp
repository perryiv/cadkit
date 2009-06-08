
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all menu commands.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Menus/Button.h"
#include "AppFrameWork/Core/BaseVisitor.h"
#include "AppFrameWork/Core/Constants.h"
#include "AppFrameWork/Core/Program.h"
#include "AppFrameWork/Core/Application.h"
#include "AppFrameWork/Windows/MainWindow.h"
#include "AppFrameWork/Menus/MenuGroup.h"
#include "AppFrameWork/Conditions/And.h"
#include "AppFrameWork/Conditions/HasEqualText.h"
#include "AppFrameWork/Conditions/IsOfType.h"
#include "AppFrameWork/Predicates/ConditionWrapper.h"

#include "Usul/Bits/Bits.h"

#include <limits>
#include <algorithm>

using namespace AFW::Menus;

AFW_IMPLEMENT_OBJECT ( Button );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Button::Button() : BaseClass(),
  _underline ( std::numeric_limits<unsigned short>::max() ),
  _type ( MENU_BUTTON )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Button::~Button()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the underline index.
//
///////////////////////////////////////////////////////////////////////////////

unsigned short Button::underline() const
{
  Guard guard ( this->mutex() );
  return _underline;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the underline index.
//
///////////////////////////////////////////////////////////////////////////////

void Button::underline ( unsigned short u )
{
  Guard guard ( this->mutex() );
  _underline = u;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Button::accept ( AFW::Core::BaseVisitor *v )
{
  Guard guard ( this->mutex() );
  if ( v )
    v->visit ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the type.
//
///////////////////////////////////////////////////////////////////////////////

Button::Type Button::type() const
{
  Guard guard ( this->mutex() );
  return _type;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the underline index.
//
///////////////////////////////////////////////////////////////////////////////

void Button::type ( const Type &t )
{
  Guard guard ( this->mutex() );
  _type = t;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Factory function.
//
///////////////////////////////////////////////////////////////////////////////

Button *Button::createSeparator()
{
  return Button::createButton ( std::string(), Button::MENU_SEPARATOR, AFW::Core::Icon() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Factory function.
//
///////////////////////////////////////////////////////////////////////////////

Button *Button::createCheck ( const std::string &text )
{
  return Button::createButton ( text, Button::MENU_CHECK, AFW::Core::Icon() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Factory function.
//
///////////////////////////////////////////////////////////////////////////////

Button *Button::createRadio ( const std::string &text )
{
  return Button::createButton ( text, Button::MENU_RADIO, AFW::Core::Icon() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Factory function.
//
///////////////////////////////////////////////////////////////////////////////

Button *Button::createButton ( const std::string &text )
{
  return Button::createButton ( text, Button::MENU_BUTTON, AFW::Core::Icon() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Factory function.
//
///////////////////////////////////////////////////////////////////////////////

Button *Button::createButton ( const std::string &text, const AFW::Core::Icon &i )
{
  return Button::createButton ( text, Button::MENU_BUTTON, i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Factory function.
//
///////////////////////////////////////////////////////////////////////////////

Button *Button::createButton ( const std::string &text, const std::string &icon )
{
  return Button::createButton ( text, Button::MENU_BUTTON, AFW::Core::Icon ( icon ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Factory function.
//
///////////////////////////////////////////////////////////////////////////////

Button *Button::createButton ( const std::string &text, const Type &t, const AFW::Core::Icon &i )
{
  Button::RefPtr button ( AFW::Core::Program::instance().newObject<Button>() );
  if ( button.valid() )
  {
    button->textSet ( text );
    button->icon ( i );
    button->type ( t );
  }
  return button.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable the window.
//
///////////////////////////////////////////////////////////////////////////////

void Button::check ( bool state )
{
  Guard guard ( this->mutex() );
  const unsigned int bit ( AFW::Core::State::ENABLED );
  this->flags ( Usul::Bits::set ( this->flags(), bit, state ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the enabled flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Button::checked() const
{
  Guard guard ( this->mutex() );
  const unsigned int bit ( AFW::Core::State::ENABLED );
  return Usul::Bits::has ( this->flags(), bit );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find and/or create the button in the menu-bar. This is a convenience 
//  function that wraps a repeated task.
//
///////////////////////////////////////////////////////////////////////////////

Button *Button::button ( const std::string &menu, const std::string &text, Type t, unsigned short u, bool create )
{
  // Don't instantiate.
  if ( false == AFW::Core::Program::valid() )
    return 0x0;

  // Get menu bar.
  AFW::Core::Application::RefPtr app ( AFW::Core::Program::instance().app() );
  AFW::Windows::MainWindow::RefPtr mw ( ( app.valid() ) ? app->mainWindow() : 0x0 );
  AFW::Menus::MenuBar::RefPtr mb ( ( mw.valid() ) ? mw->menuBar() : 0x0 );
  if ( false == mb.valid() )
    return 0x0;

  // Get or make window menu.
  AFW::Menus::MenuGroup::RefPtr wm ( mb->menu ( menu, create ) );
  if ( false == wm.valid() )
    return 0x0;

  // Look for existing button.
  AFW::Menus::Button::RefPtr b;
  {
    AFW::Conditions::And::RefPtr condition ( new AFW::Conditions::And );
    condition->append ( new AFW::Conditions::HasEqualText ( text ) );
    condition->append ( new AFW::Conditions::IsOfType < AFW::Menus::Button >() );
    AFW::Windows::Group::Itr i ( std::find_if ( wm->begin(), wm->end(), AFW::Predicates::ConditionWrapper ( condition.get() ) ) );
    b = ( ( wm->end() == i ) ? 0x0 : dynamic_cast < AFW::Menus::Button * > ( i->get() ) );
  }

  // Make new button if needed.
  if ( false == b.valid() && true == create )
  {
    AFW::Core::Program &factory ( AFW::Core::Program::instance() );
    b = factory.newObject<AFW::Menus::Button>();
    if ( false == b.valid() )
      return 0x0;
  }

  // Return now if button is still not valid.
  if ( false == b.valid() )
    return 0x0;

  // Add entry to show/hide text window.
  b->textSet ( text );
  b->underline ( u );
  b->type ( t );
  wm->append ( b.get() );

  // Return button.
  return b.get();
}
