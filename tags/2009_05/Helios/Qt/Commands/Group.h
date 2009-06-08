
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
//  Group of commands.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_COMMANDS_COMMAND_GROUP_CLASS_H_
#define _CADKIT_HELIOS_QT_COMMANDS_COMMAND_GROUP_CLASS_H_

#include "Helios/Qt/Commands/Command.h"

#include <vector>


namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT Group : public CadKit::Helios::Commands::Command
{
public:

  // Typedefs.
  typedef CadKit::Helios::Commands::Command BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::vector<Usul::Commands::Command::RefPtr> CommandList;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( Group );

  // Constructor.
  Group ( IUnknown *caller );

  // Return a copy of the commands.
  CommandList                 commands() const;

  // Is the group empty?
  bool                        empty() const;

  // Read the macro from file.
  void                        readMacro ( const std::string &file );

protected:

  // Use reference counting.
  virtual ~Group();

  Group ( const Group & );
  Group &operator = ( const Group & );

  virtual void                _execute();

private:

  void                        _destroy();

  CommandList _commands;
};


} // namespace Commands
} // namespace Helios
} // namespace CadKit


#endif // _CADKIT_HELIOS_QT_COMMANDS_COMMAND_GROUP_CLASS_H_
