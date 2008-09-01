
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Check button class for menus.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Menu/CheckButton.h"
#include "FoxTools/Headers/MenuCheck.h"

using namespace FoxTools;
using namespace FoxTools::Menu;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CheckButton::CheckButton ( unsigned int token ) : BaseClass ( token )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CheckButton::CheckButton ( const std::string &name, FX::FXObject *target, unsigned int selector, unsigned int token ) : 
  BaseClass ( name, target, selector, token )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CheckButton::CheckButton ( const std::string &name, 
                 const std::string &hotKeys, 
                 const std::string &description, 
                 FX::FXObject *target, 
                 unsigned int selector, 
                 unsigned int token ) : 
  BaseClass ( name, hotKeys, description, target, selector, token )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CheckButton::~CheckButton()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the button.
//
///////////////////////////////////////////////////////////////////////////////

void CheckButton::_build ( FX::FXComposite *parent )
{
  if ( 0x0 == this->_button() )
  {
    std::ostringstream text;
    text << this->name() << '\t' << this->hotKeys() << '\t' << this->description();
    this->_button ( new FX::FXMenuCheck ( parent, text.str().c_str(), this->target(), this->selector() ) );
    this->_button()->setUserData ( this );
  }
}