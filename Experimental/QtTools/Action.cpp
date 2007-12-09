
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

#include "QtTools/Action.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Threads/Named.h"
#include "Usul/Trace/Trace.h"

using namespace QtTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Action::Action ( Usul::Commands::Command *command ) : BaseClass ( 0x0 ),
  _command ( command )
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
      QtTools::Image::icon ( command->iconPath().c_str(), this );
  }

  this->connect ( this, SIGNAL ( triggered() ), this, SLOT ( _slot() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Action::~Action()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called by the GUI system.
//
///////////////////////////////////////////////////////////////////////////////

void Action::_slot()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Action::_execute ), "2485487655" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the command.
//
///////////////////////////////////////////////////////////////////////////////

void Action::_execute()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "3551910933" );
  
  if ( _command.valid () )
  {
    Usul::Commands::Command::RefPtr command ( _command->clone () );
    command->execute ( 0x0 );
  }
}
