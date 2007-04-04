
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

#include "pqxx/pqxx"

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

void Controller::removeLayer( Minerva::Core::Layers::Layer *layer )
{
  int eventId ( this->_executeLayerQuery( layer ) );

  this->_executeEventTableQuery(3, eventId);
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
  // Create the xml string.
  std::string xml ( Minerva::Core::serialize( layer ) );
  
  // Enter the data into the database.
  typedef Minerva::Core::DB::Connection::Values Values;
  Values values;
  values.push_back( Values::value_type ( "session_id",  Detail::toString ( _sessionID ) ) );
  values.push_back( Values::value_type ( "xml_data",    xml ) );

  int id ( _connection->executeInsertQuery("wnv_layers", values ) );
  
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

