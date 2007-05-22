
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

  // Constructor.
  template < class CallerType > Action ( CallerType *caller ) : BaseClass ( IUnknown::QueryPtr ( caller ) )
  {
    USUL_TRACE_SCOPE;
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
    USUL_THREADS_ENSURE_GUI_THREAD ( throw std::runtime_error ( "Error 3551910933: Not GUI thread" ) );
    CommandType::RefPtr command ( new CommandType ( this->caller() ) );
    command->execute ( 0x0 );
  }
};


} // namespace Commands
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_COMMANDS_ACTION_CLASS_H_
