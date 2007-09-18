
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Action class that executes a command.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_COMMANDS_ACTION_CLASS_H_
#define _CADKIT_HELIOS_QT_COMMANDS_ACTION_CLASS_H_

#include "Helios/Qt/Commands/BaseAction.h"
#include "Helios/Qt/Tools/Image.h"

#include "Usul/Commands/History.h"
#include "Usul/Threads/Named.h"
#include "Usul/Trace/Trace.h"

#include <stdexcept>


namespace CadKit {
namespace Helios {
namespace Commands {


class Action : public CadKit::Helios::Commands::BaseAction
{
public:

  // Typedefs.
  typedef CadKit::Helios::Commands::BaseAction BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Constructor.
  Action ( Usul::Commands::Command *command ) : BaseClass ( 0x0 ), _command ( command )
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
        CadKit::Helios::Tools::Image::icon ( command->iconPath().c_str(), this );
    }
  }

  // Destructor.
  virtual ~Action()
  {
    USUL_TRACE_SCOPE;
  }

private:

  // No copying or assignment.
  Action ( const Action & );
  Action &operator = ( const Action & );

  virtual void _execute()
  {
    USUL_TRACE_SCOPE;
    USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "3551910933" );
    
    if ( _command.valid () )
    {
      Usul::Commands::Command::RefPtr command ( _command->clone () );
      command->execute ( 0x0 );
    }
  }

  Usul::Commands::Command::RefPtr _command;
};


} // namespace Commands
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_COMMANDS_ACTION_CLASS_H_
