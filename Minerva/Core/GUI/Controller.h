
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

// This needs to be included before any boost/serialization headers.
#include "boost/archive/xml_oarchive.hpp"

#include "boost/serialization/base_object.hpp"
#include "boost/serialization/nvp.hpp"

#include "pqxx/pqxx"

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

  /// Get tables that contain polygon data.
  Strings          getPolygonTables();

  /// Get all column names for given table.
  Strings          getColumnNames( const std::string& table );

  /// Get all tables that contain line data.
  Strings          getLineTables();

  /// Get all tables tat contain point data.
  Strings          getPointTables();

  /// Get the name of the tables that have point data and have a data column.
  Strings          getPointTimeTables();

  /// Return true if table has a column of given type.
  bool             hasColumnType( const std::string& tableName, const std::string& type );

protected:
  virtual ~Controller();

  /// Get the geometry type from the table name.  Assumes that the table has a column named geom.
  std::string      _getGeometryType( const std::string& tableName );

  /// Delete everything in given table name.
  void             _clearTable( const std::string& tableName );

  /// Get all tables that have a geometry column.
  Strings          _getGeometryTables();

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
