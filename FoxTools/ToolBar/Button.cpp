
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/ToolBar/Button.h"
#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/ToolBar.h"
#include "FoxTools/Headers/ToggleButton.h"
#include "FoxTools/Headers/Icon.h"

#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


using namespace FoxTools::ToolBar;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Button::Button ( const std::string &name, 
                 const std::string &description, 
                 unsigned int iconId,
                 FX::FXObject *target, 
                 unsigned int selector, 
                 unsigned int token ) : 
  BaseClass ( name, "", description, target, selector, token ),
  _button ( 0x0 ),
  _iconId  ( iconId )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Button::~Button()
{
  if( _button )
  {
    _button->setUserData ( 0x0 );
    delete _button;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the button.
//
///////////////////////////////////////////////////////////////////////////////

void Button::clear()
{
  if( _button )
  {
    _button->setUserData ( 0x0 );
    delete _button;
  }
  _button = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the button.
//
///////////////////////////////////////////////////////////////////////////////

void Button::_build ( FX::FXComposite *parent )
{
  if ( 0x0 == _button )
  {
    std::ostringstream text;
    text << "\t" << this->name() << '\t' << this->description();
    
    // Make the icon.
    std::auto_ptr<FX::FXIcon> icon ( FoxTools::Icons::Factory::instance()->icon ( _iconId ) );

    // Make the button.
    unsigned int layout ( FX::BUTTON_NORMAL | FX::BUTTON_TOOLBAR );
    _button = new FX::FXButton ( parent, text.str().c_str(), icon.get(), this->target(), this->selector(), layout );

    // Release the icon, the button owns it.
    icon.release();
    
    _button->setUserData ( this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create.
//
///////////////////////////////////////////////////////////////////////////////

void Button::_create ()
{
  // Create the button if we can.
  if ( _button && !_button->id() )
    _button->create();
}

