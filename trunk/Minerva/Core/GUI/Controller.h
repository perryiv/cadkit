
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __GUI_CONTROLLER_H__
#define __GUI_CONTROLLER_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/DB/Connection.h"

#include "OsgTools/Animate/Settings.h"

#include "Serialize/XML/Macros.h"

#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include <vector>
#include <string>

namespace osg { class Vec3f; }
namespace Usul { namespace Interfaces { struct ILayer; struct ICommand; } }

namespace Minerva {
namespace Core {
namespace GUI {

class MINERVA_EXPORT Controller : public Usul::Base::Referenced
{
public:
  /// Typedef(s).
  typedef std::vector<std::string>                  Strings;
  typedef Usul::Threads::RecursiveMutex             Mutex;
  typedef Usul::Threads::Guard<Mutex>               Guard;

  USUL_DECLARE_REF_POINTERS ( Controller );

  Controller();
  Controller(const std::string& database, const std::string& user, const std::string& password, const std::string& host );

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

  /// Remove layer with given id.
  void             removeLayer( Usul::Interfaces::ILayer *Layer );

  /// Show layer.
  int              showLayer ( Usul::Interfaces::ILayer *Layer );

  /// Modify polygon data.
  void             modifyLayer( Usul::Interfaces::ILayer *layer );

  /// Start animation.
  void             startAnimation( float speed, bool accumulate, bool dateTimeStep, bool timeWindow, int numDays, OsgTools::Animate::Settings::TimestepType type );

  /// Play a movie.
  void             playMovie ( const osg::Vec3f& position, const osg::Vec3f& width, const osg::Vec3f& height, const std::string& path );

  /// Get the mutex.
  Mutex&           mutex() const { return _mutex; }
protected:
  virtual ~Controller();

  /// Delete everything in given table name.
  void             _clearTable( const std::string& tableName );

  /// Build a query for the draw command table
  int              _executeLayerQuery( Usul::Interfaces::ILayer *Layer );

  /// Add an event.
  void             _executeEventTableQuery( int type, int eventId );

private:

  Minerva::Core::DB::Connection::RefPtr _connection;
  unsigned int _sessionID;
  bool _connected;
  mutable Mutex _mutex;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( Controller );
};

}
}
}

#endif // __GUI_CONTROLLER_H__
