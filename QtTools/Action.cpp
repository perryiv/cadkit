
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

#include "QtTools/Action.h"
#include "QtTools/Image.h"

#include "Usul/Functions/SafeCall.h"
#include "Usul/Threads/Named.h"
#include "Usul/Trace/Trace.h"

#include "boost/bind.hpp"

using namespace QtTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Action::Action ( Usul::Commands::Command *command ) : BaseClass ( 0x0 ),
  _executeCallback()
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != command )
  {
    const std::string text      ( command->text() );
    const std::string shortcut  ( command->shortcut() );
    const std::string statusTip ( command->statusTip() );
    const std::string toolTip   ( command->toolTip() );
    const std::string iconPath  ( command->iconPath() );

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

    typedef Usul::Commands::Command Command;
    _executeCallback = boost::bind ( &Command::execute, Command::RefPtr ( command ), static_cast<Usul::Interfaces::IUnknown*> ( 0x0 ) );
  }

  this->connect ( this, SIGNAL ( triggered() ), this, SLOT ( _slot() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Action::Action ( const std::string& text, ExecuteCallback callback ) : BaseClass ( 0x0 ),
  _executeCallback ( callback )
{
  USUL_TRACE_SCOPE;
  
  if ( false == text.empty() )
    this->setText ( tr ( text.c_str() ) );

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
  Usul::Functions::safeCall ( boost::bind (&Action::_execute, this ), "2485487655" );
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
  
  if ( _executeCallback )
  {
    _executeCallback();
  }
}
