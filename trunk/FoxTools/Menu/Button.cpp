
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

#include <memory>

#include "fxver.h"
#include "fxdefs.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXString.h"
#include "FXStream.h"
#include "FXToolBar.h"
#include "FXIcon.h"
#include "FXButton.h"
#include "FXToggleButton.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Add the toolbar button.
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
  std::auto_ptr<FX::FXIcon> icon ( FoxTools::Icons::Factory::instance()->icon ( iconId, parent->getApp() ) );

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
  std::auto_ptr<FX::FXIcon> checkIcon   ( FoxTools::Icons::Factory::instance()->icon ( checkedIconId,   parent->getApp() ) );
  std::auto_ptr<FX::FXIcon> uncheckIcon ( FoxTools::Icons::Factory::instance()->icon ( uncheckedIconId, parent->getApp() ) );

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
