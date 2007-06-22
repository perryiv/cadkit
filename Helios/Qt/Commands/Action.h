
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


template < class CommandType > class Action : public BaseAction
{
public:

  // Typedefs.
  typedef BaseAction BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Action<CommandType> ThisType;
  typedef boost::shared_ptr<ThisType> RefPtr;

  // Constructor.
  template < class CallerType > Action ( CallerType *caller ) : BaseClass ( IUnknown::QueryPtr ( caller ) )
  {
    USUL_TRACE_SCOPE;
    CommandType::RefPtr command ( new CommandType ( this->caller() ) );

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
      CadKit::Helios::Tools::Image::icon ( command->iconPath().c_str(), this );
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
    CommandType::RefPtr command ( new CommandType ( this->caller() ) );
    command->execute ( 0x0 );
  }
};


} // namespace Commands
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_COMMANDS_ACTION_CLASS_H_
