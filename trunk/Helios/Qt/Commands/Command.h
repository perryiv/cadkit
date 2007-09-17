
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
//  Base class for all commands.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_COMMANDS_COMMAND_CLASS_H_
#define _CADKIT_HELIOS_QT_COMMANDS_COMMAND_CLASS_H_

#include "Helios/Qt/Commands/Export.h"

#include "Usul/Commands/Command.h"

#include <string>


namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT Command : public Usul::Commands::Command
{
public:

  // Typedefs.
  typedef Usul::Commands::Command BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Command );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( Command );

protected:

  // Constructor.
  Command ( IUnknown *caller );

  // Use reference counting.
  virtual ~Command();

  Command ( const Command & );
  Command &operator = ( const Command & );

private:

  void                      _destroy();
};


} // namespace Commands
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_COMMANDS_COMMAND_CLASS_H_
