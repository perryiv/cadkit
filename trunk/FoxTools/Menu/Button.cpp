
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Button class for menus.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Menu/Button.h"
#include "FoxTools/Menu/Group.h"
#include "FoxTools/Functions/Enable.h"
#include "FoxTools/Headers/MenuCommand.h"

using namespace FoxTools;
using namespace FoxTools::Menu;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Button::Button ( unsigned int token ) : BaseClass ( token ),
  _command ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Button::Button ( const std::string &name, FX::FXObject *target, unsigned int selector, unsigned int token ) : 
  BaseClass ( name, target, selector, token ),
  _command ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Button::Button ( const std::string &name, 
                 const std::string &hotKeys, 
                 const std::string &description, 
                 FX::FXObject *target, 
                 unsigned int selector, 
                 unsigned int token ) : 
  BaseClass ( name, hotKeys, description, target, selector, token ),
  _command ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Button::~Button()
{
  if( _command )
    delete _command;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the button.
//
///////////////////////////////////////////////////////////////////////////////

void Button::clear()
{
  if( _command )
    delete _command;
  _command = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the button.
//
///////////////////////////////////////////////////////////////////////////////

void Button::_build ( FX::FXComposite *parent )
{
  if ( 0x0 == this->_button() )
  {
    std::ostringstream text;
    text << this->name() << '\t' << this->hotKeys() << '\t' << this->description();
    this->_button ( new FX::FXMenuCommand ( parent, text.str().c_str(), 0x0, this->target(), this->selector() ) );
    this->_button()->setUserData ( this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable/disable the internal FOX window.
//
///////////////////////////////////////////////////////////////////////////////

void Button::enable ( bool state )
{
  FoxTools::Functions::enable ( state, _command );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the button-command.
//
///////////////////////////////////////////////////////////////////////////////

void Button::_button ( FX::FXMenuCommand *b )
{
  this->clear();
  _command = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the button-command.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXMenuCommand *Button::_button()
{
  return _command;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create.
//
///////////////////////////////////////////////////////////////////////////////

void Button::_create ()
{
  // Create the button if we can.
  if ( _command && !_command->id() )
    _command->create();
}

