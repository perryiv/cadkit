
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_COMMAND_SENDER_H__
#define __MINERVA_COMMAND_SENDER_H__

#include "Minerva/DataSources/Connection.h"

#include "Serialize/XML/Macros.h"

#include "Usul/Base/Object.h"

#include <vector>
#include <string>

namespace Usul { namespace Interfaces { struct ICommand; } }

namespace Minerva {
namespace Document {

class CommandSender : public Usul::Base::Object
{
public:
  /// Typedef(s).
  typedef Usul::Base::Object            BaseClass;
  typedef std::vector < std::string >   Strings;

  USUL_DECLARE_REF_POINTERS ( CommandSender );

  CommandSender();

  /// Set the connection.
  void             connection ( Minerva::DataSources::Connection * connection );

  /// Are we connected to the session?
  bool             connected () const;

  /// Connect to a session.
  void             connectToSession( const std::string& name );

  /// Delete the session.
  void             deleteSession();

  /// Get all availabe sessions.
  Strings          getAvailableSessions();

  /// Send a command.
  void             sendCommand ( Usul::Interfaces::ICommand *command );

protected:
  virtual ~CommandSender();

  /// Delete everything in given table name.
  void             _clearTable( const std::string& tableName );

private:

  Minerva::DataSources::Connection::RefPtr _connection;
  unsigned int _sessionID;
  bool _connected;
};


}
}

#endif // __MINERVA_COMMAND_SENDER_H__
