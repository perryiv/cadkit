
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/GUI/Controller.h"

#pragma warning ( disable : 4561 )
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/replace.hpp"

#include "Minerva/Core/Layers/LineLayer.h"
#include "Minerva/Core/Layers/PolygonLayer.h"
#include "Minerva/Core/Layers/PointLayer.h"
#include "Minerva/Core/Layers/PointTimeLayer.h"
#include "Minerva/Core/Layers/RLayer.h"
#include "Minerva/Core/Layers/PolygonTimeLayer.h"
#include "Minerva/Core/Functors/SingleColorFunctor.h"
#include "Minerva/Core/Functors/GradientColorFunctor.h"

#include <iostream>

using namespace Minerva::Core::GUI;

namespace Detail
{
  template < class T >
  std::string toString( T t )
  {
    std::ostringstream os;
    os << t;
    return os.str();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Controller::Controller( const std::string& database, const std::string& user, const std::string& password, const std::string& host ) :
_connection ( new Minerva::Core::DB::Connection ),
_sessionID( 0 )
{
  _connection->database( database );
  _connection->hostname( host );
  _connection->username ( user );
  _connection->password( password );
  
  _connection->connect();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Controller::~Controller()
{
  _connection->disconnect();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect to a session.
//
///////////////////////////////////////////////////////////////////////////////

int Controller::connectToSession( const std::string& name )
{
  std::ostringstream query;
  query << "SELECT * FROM wnv_sessions WHERE name = '" << name << "'";

  pqxx::result result ( _connection->executeQuery ( query.str() ) );

  if ( !result.empty() )
  {
    _sessionID = result[0]["id"].as < unsigned int > ();
  }
  else
  {
    // Clear the query.
    query.str("");

    query << "INSERT INTO wnv_sessions ( name ) VALUES ( '" << name << "' )";

    _connection->executeQuery( query.str() );

    // Try and connect again.
    this->connectToSession ( name );
  }

  return _sessionID;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the session.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::deleteSession()
{
  this->_clearTable("wnv_layers");
  this->_clearTable("wnv_event_table");
  this->_clearTable("wnv_animate_table");
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all availabe sessions.
//
///////////////////////////////////////////////////////////////////////////////

Controller::Strings Controller::getAvailableSessions()
{
  std::string query ( "SELECT * FROM wnv_sessions ORDER BY name" );

  pqxx::result result ( _connection->executeQuery ( query ) );

  Strings sessions;

  for ( pqxx::result::const_iterator iter = result.begin(); iter != result.end(); ++iter )
  {
    sessions.push_back ( iter["name"].as < std::string > () );
  }

  return sessions;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete everything in given table name.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::_clearTable( const std::string& tableName )
{
  std::ostringstream query;
  query << "DELETE FROM " << tableName << " WHERE session_id = " << _sessionID;

  _connection->executeQuery ( query.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add an event.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::_executeEventTableQuery( int type, int eventId )
{
  typedef Minerva::Core::DB::Connection::Values Values;
  Values values;
  values.push_back ( Values::value_type ( "type", Detail::toString ( type ) ) );
  values.push_back ( Values::value_type ( "event_id", Detail::toString ( eventId ) ) );
  values.push_back ( Values::value_type ( "session_id", Detail::toString ( _sessionID ) ) );

  _connection->executeInsertQuery("wnv_event_table", values);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove layer with given id.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::removeLayer(int layerID)
{
  this->_executeEventTableQuery(3, layerID);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify polygon data.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::modifyLayer( Minerva::Core::Layers::Layer *layer )
{
  this->showLayer( layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a query for the draw command table.
//
///////////////////////////////////////////////////////////////////////////////

int Controller::_executeLayerQuery( Minerva::Core::Layers::Layer *layer )
{
  // Create the archive.
  std::ostringstream os;
  //boost::archive::xml_oarchive oa ( os );
  //
  //// Register types.
  //oa.register_type<Minerva::Core::Layers::LineLayer>();
  //oa.register_type<Minerva::Core::Layers::PolygonLayer>();
  //oa.register_type<Minerva::Core::Layers::PointLayer>();
  //oa.register_type<Minerva::Core::Layers::PointTimeLayer>();
  //oa.register_type<Minerva::Core::Layers::RLayer>();
  //oa.register_type<Minerva::Core::Functors::SingleColorFunctor>();
  //oa.register_type<Minerva::Core::Functors::GradientColorFunctor>();
  //oa.register_type<Minerva::Core::Layers::PolygonTimeLayer>();

  //// Serialize.
  //oa << boost::serialization::make_nvp( "Layer", layer );
  std::string xml ( os.str() );
  
  // Enter the data into the database.
  typedef Minerva::Core::DB::Connection::Values Values;
  Values values;
  values.push_back( Values::value_type ( "session_id",  Detail::toString ( _sessionID ) ) );
  values.push_back( Values::value_type ( "xml_data",    xml ) );

  int id ( _connection->executeInsertQuery("wnv_layers", values ) );
  //layer->layerID( id );
  return id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show layer.
//
///////////////////////////////////////////////////////////////////////////////

int Controller::showLayer ( Minerva::Core::Layers::Layer *layer )
{
  int eventId ( this->_executeLayerQuery( layer ) );

  this->_executeEventTableQuery(1, eventId);

  return eventId;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start animation.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::startAnimation(float speed, bool accumulate, bool dateTimeStep, bool timeWindow, int numDays )
{
  typedef Minerva::Core::DB::Connection::Values Values;
  Values values;

  values.push_back ( Values::value_type ( "animate", "true" ) );
  values.push_back ( Values::value_type ( "session_id", Detail::toString ( _sessionID ) ) );
  values.push_back ( Values::value_type ( "speed", Detail::toString ( speed ) ) );
  values.push_back ( Values::value_type ( "accumulate", Detail::toString ( accumulate ) ) );
  values.push_back ( Values::value_type ( "date_time_step", Detail::toString ( dateTimeStep ) ) );
  values.push_back ( Values::value_type ( "time_window", Detail::toString ( timeWindow ) ) );
  values.push_back ( Values::value_type ( "num_days_to_show", Detail::toString ( numDays ) ) );

  int eventId ( _connection->executeInsertQuery("wnv_animate_table", values) );

  this->_executeEventTableQuery(2, eventId);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop Animation.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::stopAnimation()
{
  typedef Minerva::Core::DB::Connection::Values Values;
  Values values;

  values.push_back ( Values::value_type ( "animate", "false" ) );
  values.push_back ( Values::value_type ( "session_id", Detail::toString ( _sessionID ) ) );

  int eventId ( _connection->executeInsertQuery("wnv_animate_table", values) );

  this->_executeEventTableQuery(2, eventId);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get tables that contain polygon data.
//
///////////////////////////////////////////////////////////////////////////////

Controller::Strings Controller::getPolygonTables()
{
  Strings strings;

  Strings tableNames ( this->_getGeometryTables() );

  for ( Strings::const_iterator iter = tableNames.begin(); iter != tableNames.end(); ++iter )
  {
    std::string table ( *iter );
    std::string query ( "SELECT GeometryType(geom) FROM " + table + " LIMIT 1" );

    pqxx::result result ( _connection->executeQuery( query ) );

    if( !result.empty() )
    {
      std::string geomType ( result[0][0].as < std::string > () );
      
      if (geomType == "POLYGON" || geomType == "MULTIPOLYGON")
      {
        strings.push_back( table );
      }
    }
  }

  return strings;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all column names for given table.
//
///////////////////////////////////////////////////////////////////////////////

Controller::Strings Controller::getColumnNames( const std::string& table)
{
  Strings strings;

  std::string query ( "SELECT column_name FROM information_schema.columns WHERE table_name='" + table + "'" );

  pqxx::result result ( _connection->executeQuery ( query ) );

  for( pqxx::result::const_iterator iter = result.begin(); iter != result.end(); ++iter )
  {
    strings.push_back( iter[0].as< std::string > () );
  }

  return strings;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all tables that contain line data.
//
///////////////////////////////////////////////////////////////////////////////

Controller::Strings Controller::getLineTables()
{
  Strings strings;

  Strings tableNames ( this->_getGeometryTables() );

  for ( Strings::const_iterator iter = tableNames.begin(); iter != tableNames.end(); ++iter )
  {
    std::string table ( *iter );
    std::string query ( "SELECT GeometryType(geom) FROM " + table + " LIMIT 1" );

    pqxx::result result ( _connection->executeQuery( query ) );

    if( !result.empty() )
    {
      std::string geomType ( result[0][0].as < std::string > () );

      if (geomType == "POLYGON" || geomType == "MULTIPOLYGON" || geomType == "LINESTRING" || geomType == "MULTILINESTRING" )
      {
        strings.push_back ( table );
      }
    }
  }

  return strings;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all tables tat contain point data.
//
///////////////////////////////////////////////////////////////////////////////

Controller::Strings Controller::getPointTables()
{
  Strings strings;

  Strings tableNames ( this->_getGeometryTables() );

  for ( Strings::const_iterator iter = tableNames.begin(); iter != tableNames.end(); ++iter )
  {
    std::string table ( *iter );

    std::string query ( "SELECT GeometryType(geom) FROM " + table + " WHERE IsEmpty(geom)=false LIMIT 1" );
    pqxx::result result ( _connection->executeQuery( query ) );

    if( !result.empty() )
    {
      std::string geomType ( result[0][0].as < std::string > () );

      if (geomType == "POINT" || geomType == "MULTIPOINT" )
      {
        strings.push_back ( table );
      }
    }
  }

  return strings;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name of the tables that have point data and have a data column.
//
///////////////////////////////////////////////////////////////////////////////

Controller::Strings Controller::getPointTimeTables()
{
  Strings strings;

  Strings tableNames ( this->_getGeometryTables() );

  for ( Strings::const_iterator iter = tableNames.begin(); iter != tableNames.end(); ++iter )
  {
    std::string table ( *iter );

    // If this table has a date column...
    if ( this->hasColumnType( table, "date") )
    {
      std::string geomType ( this->_getGeometryType(table) );

      if ( geomType == "POINT" || geomType == "MULTIPOINT" )
      {
        strings.push_back(table);
      }
    }
  }

  return strings;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all tables that have a geometry column.
//
///////////////////////////////////////////////////////////////////////////////

Controller::Strings Controller::_getGeometryTables()
{
  Strings strings;

  std::string query ( "SELECT table_name FROM information_schema.columns WHERE column_name=\'geom\'" );
  pqxx::result result ( _connection->executeQuery( query ) );

  for( pqxx::result::const_iterator iter = result.begin(); iter != result.end(); ++iter )
  {
    strings.push_back( iter["table_name"].as< std::string > () );
  }

  return strings;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Return true if table has a column of given type.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::hasColumnType( const std::string& tableName, const std::string& type )
{
  Strings columnNames ( this->getColumnNames(tableName) );

  for ( Strings::const_iterator iter = columnNames.begin(); iter != columnNames.end(); ++iter )
  {
    std::string query ( "SELECT data_type FROM information_schema.columns WHERE table_name='" + tableName + "' AND column_name='" + *iter + "'" );

    pqxx::result result ( _connection->executeQuery( query ) );

    if( !result.empty() )
    {
      std::string columnType ( result[0]["data_type"].as< std::string > () );
      boost::algorithm::trim(columnType);

      if (columnType == type)
        return true;
    }
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the geometry type from the table name.  Assumes that the table has a column named geom.
//
///////////////////////////////////////////////////////////////////////////////

std::string Controller::_getGeometryType( const std::string& tableName )
{
  std::string query ( "SELECT GeometryType(geom) FROM " + tableName + " WHERE IsEmpty(geom)=false LIMIT 1" );

  pqxx::result result ( _connection->executeQuery( query ) );

  if( !result.empty() )
  {
    return result[0][0].is_null() ? "" : result[0][0].as < std::string > ();
  }

  return "";
}
