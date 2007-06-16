
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/GUI/Controller.h"
#include "Minerva/Core/Serialize.h"

#ifdef _MSC_VER
# pragma warning ( disable : 4561 )
#endif

#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/replace.hpp"

#include "pqxx/pqxx"

#include <iostream>

using namespace Minerva::Core::GUI;

std::ostream& operator<<( std::ostream& os, const osg::Vec3f& v )
{
  os << v.x() << " " << v.y() << " " << v.z();
  return os;
}

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

Controller::Controller( ) :
_connection ( new Minerva::Core::DB::Connection ),
_sessionID( 0 ),
_connected ( false ),
_mutex()
{
  SERIALIZE_XML_ADD_MEMBER ( _connection );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Controller::Controller( const std::string& database, const std::string& user, const std::string& password, const std::string& host ) :
_connection ( new Minerva::Core::DB::Connection ),
_sessionID( 0 ),
_connected ( false ),
_mutex()
{
  _connection->database( database );
  _connection->hostname( host );
  _connection->username ( user );
  _connection->password( password );
  
  _connection->connect();

  SERIALIZE_XML_ADD_MEMBER ( _connection );
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
//  Are we connected to the session?
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::connected() const
{
  return _connected;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect to a session.
//
///////////////////////////////////////////////////////////////////////////////

int Controller::connectToSession( const std::string& name )
{
  Guard guard ( this->mutex() );

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

  _connected = true;
  return _sessionID;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the session.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::deleteSession()
{
  Guard guard ( this->mutex() );

  this->_clearTable("play_movie");
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
  Guard guard ( this->mutex() );

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
  Guard guard ( this->mutex() );

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
  Guard guard ( this->mutex() );

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

void Controller::removeLayer( Usul::Interfaces::ILayer *layer )
{
  Guard guard ( this->mutex() );

  int eventId ( this->_executeLayerQuery( layer ) );

  this->_executeEventTableQuery(3, eventId);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify polygon data.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::modifyLayer( Usul::Interfaces::ILayer *layer )
{
  Guard guard ( this->mutex() );

  this->showLayer( layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a query for the draw command table.
//
///////////////////////////////////////////////////////////////////////////////

int Controller::_executeLayerQuery( Usul::Interfaces::ILayer *layer )
{
  Guard guard ( this->mutex() );

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

int Controller::showLayer ( Usul::Interfaces::ILayer *layer )
{
  Guard guard ( this->mutex() );

  int eventId ( this->_executeLayerQuery( layer ) );

  this->_executeEventTableQuery(1, eventId);

  return eventId;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start animation.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::startAnimation(float speed, bool accumulate, bool dateTimeStep, bool timeWindow, int numDays, OsgTools::Animate::Settings::TimestepType type )
{
  Guard guard ( this->mutex() );

  typedef Minerva::Core::DB::Connection::Values Values;
  Values values;

  values.push_back ( Values::value_type ( "animate", "true" ) );
  values.push_back ( Values::value_type ( "session_id", Detail::toString ( _sessionID ) ) );
  values.push_back ( Values::value_type ( "speed", Detail::toString ( speed ) ) );
  values.push_back ( Values::value_type ( "accumulate", Detail::toString ( accumulate ) ) );
  values.push_back ( Values::value_type ( "date_time_step", Detail::toString ( dateTimeStep ) ) );
  values.push_back ( Values::value_type ( "time_window", Detail::toString ( timeWindow ) ) );
  values.push_back ( Values::value_type ( "num_days_to_show", Detail::toString ( numDays ) ) );
  values.push_back ( Values::value_type ( "timestep_type", Detail::toString ( type ) ) );

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
  Guard guard ( this->mutex() );

  typedef Minerva::Core::DB::Connection::Values Values;
  Values values;

  values.push_back ( Values::value_type ( "animate", "false" ) );
  values.push_back ( Values::value_type ( "session_id", Detail::toString ( _sessionID ) ) );

  int eventId ( _connection->executeInsertQuery("wnv_animate_table", values) );

  this->_executeEventTableQuery(2, eventId);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play Movie.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::playMovie ( const osg::Vec3f& position, const osg::Vec3f& width, const osg::Vec3f& height, const std::string& path )
{
  Guard guard ( this->mutex() );

  typedef Minerva::Core::DB::Connection::Values Values;
  Values values;

  values.push_back ( Values::value_type ( "session_id", Detail::toString ( _sessionID ) ) );
  values.push_back ( Values::value_type ( "the_position", Detail::toString ( position ) ) );
  values.push_back ( Values::value_type ( "width", Detail::toString ( width ) ) );
  values.push_back ( Values::value_type ( "height", Detail::toString ( height ) ) );
  values.push_back ( Values::value_type ( "path", Detail::toString ( path ) ) );

  int eventId ( _connection->executeInsertQuery("play_movie", values) );

  this->_executeEventTableQuery(4, eventId);
}
