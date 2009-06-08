
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/DataSources/PG/ConnectionPool.h"

#include "Usul/Registry/Database.h"

using namespace Minerva::DataSources::PG;


///////////////////////////////////////////////////////////////////////////////
//
//  String conversion for boolean.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{ 
  namespace Convert
  {
    template <> struct Type < ConnectionPool::Passwords, std::string >
    {
      typedef Type < ConnectionPool::Passwords, std::string > ThisType;
      static void convert ( ConnectionPool::Passwords from, std::string &to )
      {
        std::ostringstream in;
        for ( ConnectionPool::Passwords::const_iterator iter = from.begin(); iter != from.end(); ++iter )
        {
          in << iter->first.first << " " << iter->first.second << " " << iter->second << " ";
        }
        to = in.str();
      }
      static std::string convert ( ConnectionPool::Passwords from )
      {
        std::string to;
        ThisType::convert ( from, to );
        return to;
      }
    };
    template <> struct Type < std::string, ConnectionPool::Passwords >
    {
      typedef Type < std::string, ConnectionPool::Passwords > ThisType;
      static void convert ( const std::string &from, ConnectionPool::Passwords &to )
      {
        std::istringstream in ( from );
        while ( false == in.eof() )
        {
          std::string user, host, password;
          in >> user;
          in >> host;
          in >> password;
          to.insert ( std::make_pair ( std::make_pair ( user, host ), password ) );
        }
      }
      static ConnectionPool::Passwords convert ( const std::string &from )
      {
        ConnectionPool::Passwords to;
        ThisType::convert ( from, to );
        return to;
      }
    };
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constants for registry.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  const std::string SECTION ( "connection_pool" );
  const std::string KEY     ( "saved_passwords" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initalize static data member.
//
///////////////////////////////////////////////////////////////////////////////

ConnectionPool* ConnectionPool::_instance( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Get the instance.
//
///////////////////////////////////////////////////////////////////////////////

ConnectionPool& ConnectionPool::instance()
{
	if ( 0x0 == _instance )
		_instance = new ConnectionPool;
	return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ConnectionPool::ConnectionPool() : 
	_mutex ( Mutex::create() ),
	_connections(),
	_passwords()
{
  // Restore the passwords.  Currently this is for multi-screen set ups where a password prompt isn't avaiable.
  // Since this is stored in clear text, only save user's with read-only access.
  _passwords = Usul::Registry::Database::instance()[Detail::SECTION][Detail::KEY].get<Passwords> ( Passwords() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ConnectionPool::~ConnectionPool()
{
  // Enable this if we can encrypt passwords (and/or add a check box to the dialog).
  //Usul::Registry::Database::instance()[Detail::SECTION][Detail::KEY] = _passwords;
  
	delete _mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the pool contain a connection by this name?
//
///////////////////////////////////////////////////////////////////////////////

bool ConnectionPool::hasConnection ( const std::string& name ) const
{
	Guard guard ( *_mutex );
  Connections::const_iterator iter = _connections.find( name );
  return iter != _connections.end();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the connection for the given name.
//
///////////////////////////////////////////////////////////////////////////////

Connection* ConnectionPool::getConnection ( const std::string& name )
{
	Guard guard ( *_mutex );
  return _connections[name].get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a connection.
//
///////////////////////////////////////////////////////////////////////////////

void ConnectionPool::addConnection ( Connection* connection )
{
	Guard guard ( *_mutex );
  _connections[ connection->name() ] = connection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the password saved for this user and host.
//
///////////////////////////////////////////////////////////////////////////////

bool ConnectionPool::hasPassword ( const std::string& user, const std::string& host ) const
{
	Guard guard ( *_mutex );
	Passwords::const_iterator iter ( _passwords.find ( Passwords::key_type ( user, host ) ) );
	return ( iter != _passwords.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the password saved for this user and host.
//
///////////////////////////////////////////////////////////////////////////////

std::string ConnectionPool::getPassword ( const std::string& user, const std::string& host ) const
{
	Guard guard ( *_mutex );
	Passwords::const_iterator iter ( _passwords.find ( Passwords::key_type ( user, host ) ) );
	return ( iter != _passwords.end() ? iter->second : "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the password saved for this user and host.
//
///////////////////////////////////////////////////////////////////////////////

void ConnectionPool::setPassword ( const std::string& user, const std::string& host, const std::string& password )
{
	Guard guard ( *_mutex );
	_passwords[Passwords::key_type ( user, host )] = password;
}
