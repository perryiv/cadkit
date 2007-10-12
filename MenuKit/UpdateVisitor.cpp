
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "MenuKit/UpdateVisitor.h"
#include "MenuKit/Button.h"
#include "MenuKit/RadioButton.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/Menu.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/IUpdateEnable.h"
#include "Usul/Interfaces/IUpdateCheck.h"

using namespace MenuKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

UpdateVisitor::UpdateVisitor () : BaseClass ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

UpdateVisitor::~UpdateVisitor ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the menu.
//
///////////////////////////////////////////////////////////////////////////////

void UpdateVisitor::apply ( Menu &m )
{
  // Disable the menu if it's empty.
  m.enabled ( !m.empty () );

  // Visit all it's children.
  m.traverse ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the button.
//
///////////////////////////////////////////////////////////////////////////////

void UpdateVisitor::apply ( Button &b )
{
  Usul::Commands::Command::RefPtr command ( b.command () );

  Usul::Interfaces::IUpdateEnable::QueryPtr ue ( command );

  if ( ue.valid () )
  {
    b.enabled ( ue->updateEnable () );
  }

  if ( b.toggle () || b.radio () )
  {
    Usul::Interfaces::IUpdateCheck::QueryPtr uc ( command );
    if ( uc.valid () )
      b.checked ( uc->updateCheck () );
  }
}
