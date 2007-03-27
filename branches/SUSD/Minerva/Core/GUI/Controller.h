
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

// Must be included first.
#include "Minerva/Core/Serialize.h"

#include "Minerva/Core/Export.h"
#include "Minerva/Core/DB/Connection.h"

#include <vector>
#include <string>

namespace Minerva {
namespace Core {
  namespace Layers { class Layer; }
namespace GUI {

class MINERVA_EXPORT Controller : public Usul::Base::Referenced
{
public:
  /// Typedef(s).
  typedef std::vector<std::string> Strings;

  Controller(const std::string& database, const std::string& user, const std::string& password, const std::string& host );

  /// Connect to a session.
  int              connectToSession( const std::string& name );

  /// Delete the session.
  void             deleteSession();

  /// Get all availabe sessions.
  Strings          getAvailableSessions();

  /// Remove layer with given id.
  void             removeLayer( int layerID );

  /// Show layer.
  int              showLayer ( Minerva::Core::Layers::Layer *Layer );

  /// Modify polygon data.
  void             modifyLayer( Minerva::Core::Layers::Layer *layer );

  /// Start animation.
  void             startAnimation( float speed, bool accumulate, bool dateTimeStep, bool timeWindow, int numDays );

  /// Stop Animation.
  void             stopAnimation();

protected:
  virtual ~Controller();

  /// Delete everything in given table name.
  void             _clearTable( const std::string& tableName );

  /// Build a query for the draw command table
  int              _executeLayerQuery( Minerva::Core::Layers::Layer *Layer );

  /// Add an event.
  void             _executeEventTableQuery( int type, int eventId );

private:

  Minerva::Core::DB::Connection::RefPtr _connection;
  unsigned int _sessionID;
};

}
}
}

#endif // __GUI_CONTROLLER_H__
