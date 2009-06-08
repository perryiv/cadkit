
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Document/CommandSender.h"

#include "Minerva/DataSources/Result.h"

#include "Minerva/Core/Serialize.h"

#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/ISerialize.h"

#ifdef _MSC_VER
# pragma warning ( disable : 4561 )
#endif

#include <iostream>

using namespace Minerva::Document;


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

CommandSender::CommandSender() :
  BaseClass (),
  _connection ( 0x0 ),
  _sessionID( 0 ),
  _connected ( false )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CommandSender::~CommandSender()
{
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

void CommandSender::connectToSession( const std::string& name )
{
  Guard guard ( this->mutex() );

  std::ostringstream query;
  query << "SELECT * FROM wnv_sessions WHERE name = '" << name << "'";

  Minerva::DataSources::Result::RefPtr result ( _connection->executeQuery ( query.str() ) );

  if ( result->prepareNextRow() )
  {
    _sessionID = result->asUInt ( "id" );
    _connected = true;
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

  Minerva::DataSources::Result::RefPtr result ( _connection->executeQuery ( query ) );

  Strings sessions;

  while ( result->prepareNextRow() )
  {
    sessions.push_back ( result->asString ( "name" ) );
  }

  return sessions;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete everything in given table name.
//
///////////////////////////////////////////////////////////////////////////////

void CommandSender::_clearTable ( const std::string& tableName )
{
  Guard guard ( this->mutex() );
  
  // Return now if no connection.
  if ( false == _connection.valid() )
    return;

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
    typedef Minerva::DataSources::Connection::Values Values;
    Values values;
    values.push_back( Values::value_type ( "session_id",  Detail::toString ( _sessionID ) ) );
    values.push_back( Values::value_type ( "xml_data",    xml ) );

    _connection->executeInsertQuery( "commands", values );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the connection.
//
///////////////////////////////////////////////////////////////////////////////

void CommandSender::connection ( Minerva::DataSources::Connection * connection )
{
  Guard guard ( this->mutex () );
  _connection = connection;
}
