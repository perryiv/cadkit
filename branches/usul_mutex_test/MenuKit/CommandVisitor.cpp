
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "MenuKit/CommandVisitor.h"
#include "MenuKit/Button.h"
#include "MenuKit/RadioButton.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/Menu.h"
#include "MenuKit/MenuCommands.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/IUpdateEnable.h"
#include "Usul/Interfaces/IUpdateCheck.h"

using namespace MenuKit;

#include <iostream>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CommandVisitor::CommandVisitor () : BaseClass ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CommandVisitor::~CommandVisitor ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the menu.
//
///////////////////////////////////////////////////////////////////////////////

void CommandVisitor::apply ( Menu &m )
{
  
  // Add the menu text to the menu name stack
  _names.push_back( m.text() );

  // Visit all it's children.
  m.traverse ( *this );

  //Pop the last menu name from the stack
  _names.pop_back();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the button.
//
///////////////////////////////////////////////////////////////////////////////

void CommandVisitor::apply ( Button &b )
{
  Usul::Commands::Command::RefPtr command ( b.command () );
  
  // Add the command to the Menu command map
 
  std::string name;
  for( unsigned int i = 0; i < _names.size(); ++i )
  {
    name += _names.at( i ) + "|";
  }

  name += b.text();
  std::cout << "Name for this command is: " << name << std::endl;
  MenuKit::MenuCommands::instance().add( name, command );

}
