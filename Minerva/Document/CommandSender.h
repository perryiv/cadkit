
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

#include "Minerva/Core/Export.h"
#include "Minerva/Core/DB/Connection.h"

#include "OsgTools/Animate/Settings.h"

#include "Serialize/XML/Macros.h"

#include "Usul/Base/Object.h"

#include <vector>
#include <string>

namespace osg { class Vec3f; }
namespace Usul { namespace Interfaces { struct ILayer; struct ICommand; } }

namespace Minerva {
namespace Document {

class CommandSender : public Usul::Base::Object
{
public:
  /// Typedef(s).
  typedef Usul::Base::Object            BaseClass;
  typedef std::vector < std::string >   Strings;

  USUL_DECLARE_REF_POINTERS ( CommandSender );

  CommandSender ();
  CommandSender ( const std::string& database, const std::string& user, const std::string& password, const std::string& host );

  /// Are we connected to the session?
  bool             connected() const;

  /// Connect to a session.
  int              connectToSession( const std::string& name );

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

  Minerva::Core::DB::Connection::RefPtr _connection;
  unsigned int _sessionID;
  bool _connected;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( CommandSender );
};


}
}

#endif // __MINERVA_COMMAND_SENDER_H__
