
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions to work with menu buttons.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Menu/Button.h"
#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/ToolBar.h"
#include "FoxTools/Headers/ToggleButton.h"
#include "FoxTools/Headers/Icon.h"

#include <memory>


///////////////////////////////////////////////////////////////////////////////
//
//  Add the toolbar button with an icon.
//
///////////////////////////////////////////////////////////////////////////////

void FoxTools::Menu::addButton ( FX::FXToolBar *parent, 
                                 const std::string &text, 
                                 unsigned int iconId, 
                                 FX::FXObject *target,
                                 unsigned int commandId,
                                 void *userData )
{
  // Make the icon.
  std::auto_ptr<FX::FXIcon> icon ( FoxTools::Icons::Factory::instance()->icon ( iconId ) );

  // Make the button.
  unsigned int layout ( FX::BUTTON_NORMAL | FX::BUTTON_TOOLBAR );
  FX::FXButton *button = new FX::FXButton ( parent, text.c_str(), icon.get(), target, commandId, layout );

  // Create the new button if the parent is already created.
  if ( parent->id() )
    button->create();

  // Set the userdata if valid.
  if ( userData )
    button->setUserData ( userData );

  // Release the icon, the button owns it.
  icon.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the toolbar button.
//
///////////////////////////////////////////////////////////////////////////////

void FoxTools::Menu::addButton ( FX::FXToolBar *parent, 
                                  const std::string &text, 
                                  FX::FXObject *target,
                                  unsigned int commandId,
                                  void *userData )
{
  // Make the button.
  unsigned int layout ( FX::BUTTON_NORMAL | FX::BUTTON_TOOLBAR );
  FX::FXButton *button = new FX::FXButton ( parent, text.c_str(), 0x0, target, commandId, layout );

  // Create the new button if the parent is already created.
  if ( parent->id() )
    button->create();

  // Set the userdata if valid.
  if ( userData )
    button->setUserData ( userData );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a toggle button to the tool-bar.
//
///////////////////////////////////////////////////////////////////////////////

void FoxTools::Menu::addToggle ( FX::FXToolBar *parent, 
                                 const std::string &text, 
                                 unsigned int checkedIconId, 
                                 unsigned int uncheckedIconId, 
                                 FX::FXObject *target,
                                 unsigned int commandId,
                                 void *userData )
{
  // Make the icon.
  std::auto_ptr<FX::FXIcon> checkIcon   ( FoxTools::Icons::Factory::instance()->icon ( checkedIconId   ) );
  std::auto_ptr<FX::FXIcon> uncheckIcon ( FoxTools::Icons::Factory::instance()->icon ( uncheckedIconId ) );

  // Make the button.
  unsigned int layout ( FX::TOGGLEBUTTON_NORMAL | FX::TOGGLEBUTTON_TOOLBAR | FX::TOGGLEBUTTON_KEEPSTATE );
  FX::FXToggleButton *toggle = new FX::FXToggleButton ( parent, text.c_str(), text.c_str(), checkIcon.get(), uncheckIcon.get(), target, commandId, layout );

  // Create the new button if the parent is already created.
  if ( parent->id() )
    toggle->create();

  // Set the userdata if valid.
  if ( userData )
    toggle->setUserData ( userData );

  // Release the icons, the button owns them.
  checkIcon.release();
  uncheckIcon.release();
}
