
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
//  Command to exit the application.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_COMMANDS_EXIT_APPLICATION_COMMAND_H_
#define _CADKIT_HELIOS_QT_COMMANDS_EXIT_APPLICATION_COMMAND_H_

#include "Helios/Qt/Commands/Command.h"


namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT ExitApplication : public CadKit::Helios::Commands::Command
{
public:

  // Typedefs.
  typedef CadKit::Helios::Commands::Command BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( ExitApplication );

  // Constructor.
  ExitApplication ( IUnknown *caller );

protected:

  // Use reference counting.
  virtual ~ExitApplication();

  virtual void              _execute();

};


} // namespace Commands
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_COMMANDS_EXIT_APPLICATION_COMMAND_H_
