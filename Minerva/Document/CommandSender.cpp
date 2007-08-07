
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Document/CommandSender.h"

#include "Minerva/Core/Serialize.h"

#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/ISerialize.h"

#ifdef _MSC_VER
# pragma warning ( disable : 4561 )
#endif

#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/replace.hpp"

#include "pqxx/pqxx"

#include <iostream>

using namespace Minerva::Document;

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

CommandSender::CommandSender( ) :
BaseClass (),
_connection ( new Minerva::Core::DB::Connection ),
_sessionID( 0 ),
_connected ( false )
{
  SERIALIZE_XML_ADD_MEMBER ( _connection );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CommandSender::CommandSender( const std::string& database, const std::string& user, const std::string& password, const std::string& host ) :
BaseClass (),
_connection ( new Minerva::Core::DB::Connection ),
_sessionID( 0 ),
_connected ( false )
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

CommandSender::~CommandSender()
{
  _connection->disconnect();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we connected to the session?
//
///////////////////////////////////////////////////////////////////////////////

bool CommandSender::connected() const
{
  return _connected;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect to a session.
//
///////////////////////////////////////////////////////////////////////////////

int CommandSender::connectToSession( const std::string& name )
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

void CommandSender::deleteSession()
{
  Guard guard ( this->mutex() );

  this->_clearTable ( "commands" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all availabe sessions.
//
///////////////////////////////////////////////////////////////////////////////

CommandSender::Strings CommandSender::getAvailableSessions()
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

void CommandSender::_clearTable( const std::string& tableName )
{
  Guard guard ( this->mutex() );

  std::ostringstream query;
  query << "DELETE FROM " << tableName << " WHERE session_id = " << _sessionID;

  _connection->executeQuery ( query.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send a command.
//
///////////////////////////////////////////////////////////////////////////////

void CommandSender::sendCommand ( Usul::Interfaces::ICommand *command )
{
  Guard guard ( this->mutex() );

  Usul::Interfaces::ISerialize::QueryPtr serialize ( command );

  if( serialize.valid() )
  {
    // Create the xml string.
    std::string xml ( Minerva::Core::serialize < Usul::Interfaces::ISerialize > ( serialize.get() ) );
    
    // Enter the data into the database.
    typedef Minerva::Core::DB::Connection::Values Values;
    Values values;
    values.push_back( Values::value_type ( "session_id",  Detail::toString ( _sessionID ) ) );
    values.push_back( Values::value_type ( "xml_data",    xml ) );

    _connection->executeInsertQuery( "commands", values );
  }
}
