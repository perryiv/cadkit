
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Singleton class containing a map of names to commands
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_MENU_COMMANDS_
#define _MENUKIT_MENU_COMMANDS_

#include "MenuKit/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include <string>
#include <map>


namespace MenuKit {

class Visitor;
class Menu;

class MENUKIT_EXPORT MenuCommands
{
public:
  typedef Usul::Commands::Command::RefPtr                 CommandPtr;
  typedef std::map< std::string, CommandPtr >             Commands;
  typedef Usul::Threads::Mutex                            Mutex;
  typedef Usul::Threads::Guard < Mutex >                  Guard;

  static MenuCommands&              instance();

  // Clear the internal map.
  void                              clear();

  CommandPtr                        find ( const std::string& name ) const;
  void                              add ( const std::string& name, CommandPtr command );

  // Get the mutex. Use with caution.
  Mutex&                            mutex() const { return *_mutex; }

private:

  // Construction/Destruction.
  MenuCommands();
  ~MenuCommands();

  // Do not copy.
  MenuCommands ( const MenuCommands & );
  MenuCommands &operator = ( const MenuCommands & );

  static MenuCommands*            _instance;
  Commands                        _commandMap;

  mutable Mutex*                  _mutex;
};


} // namespace MenuKit


#endif // _MENUKIT_MENU_COMMANDS_
