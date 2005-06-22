
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/ToolBar/ToggleButton.h"
#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Headers/ToolBar.h"
#include "FoxTools/Headers/ToggleButton.h"
#include "FoxTools/Headers/Icon.h"


using namespace FoxTools::ToolBar;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ToggleButton::ToggleButton ( const std::string &name, 
                             const std::string &description, 
                             unsigned int checkedIconId, 
                             unsigned int uncheckedIconId,
                             FX::FXObject *target, 
                             unsigned int selector, 
                             unsigned int token ) : 
  BaseClass ( name, "", description, target, selector, token ),
  _button ( 0x0 ),
  _checkedIconId  ( checkedIconId ),
  _uncheckedIconId ( uncheckedIconId )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ToggleButton::ToggleButton ( const std::string &name, 
                             const std::string &description, 
                             unsigned int iconId, 
                             FX::FXObject *target, 
                             unsigned int selector, 
                             unsigned int token ) : 
  BaseClass ( name, "", description, target, selector, token ),
  _button ( 0x0 ),
  _checkedIconId  ( iconId ),
  _uncheckedIconId ( iconId )
{
}

///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ToggleButton::~ToggleButton()
{
  if( _button )
    delete _button;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the button.
//
///////////////////////////////////////////////////////////////////////////////

void ToggleButton::clear()
{
  if( _button )
    delete _button;
  _button = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the button.
//
///////////////////////////////////////////////////////////////////////////////

void ToggleButton::_build ( FX::FXComposite *parent )
{
  if ( 0x0 == _button )
  {
    std::ostringstream text;
    text << '\t' << this->name() << '\t' << this->description();
    
    // Make the icon.
    std::auto_ptr<FX::FXIcon> checkIcon   ( FoxTools::Icons::Factory::instance()->icon ( _checkedIconId   ) );
    std::auto_ptr<FX::FXIcon> uncheckIcon ( FoxTools::Icons::Factory::instance()->icon ( _uncheckedIconId ) );

    // Make the button.
    unsigned int layout ( FX::TOGGLEBUTTON_NORMAL | FX::TOGGLEBUTTON_TOOLBAR | FX::TOGGLEBUTTON_KEEPSTATE );
    _button = new FX::FXToggleButton ( parent, text.str().c_str(), text.str().c_str(), checkIcon.get(), uncheckIcon.get(), this->target(), this->selector(), layout );

    // Release the icons, the button owns them.
    checkIcon.release();
    uncheckIcon.release();
    
    _button->setUserData ( this );
  }
}

