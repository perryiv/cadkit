
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/DB/ConnectionPool.h"

using namespace Minerva::Core::DB;


///////////////////////////////////////////////////////////////////////////////
//
//  Initalize static data member.
//
///////////////////////////////////////////////////////////////////////////////

ConnectionPool* ConnectionPool::_instance( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ConnectionPool::ConnectionPool() : _connections()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ConnectionPool::~ConnectionPool()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the pool contain a connection by this name?
//
///////////////////////////////////////////////////////////////////////////////

bool ConnectionPool::hasConnection ( const std::string& name ) const
{
  Connections::const_iterator iter = _connections.find( name );
  return iter != _connections.end();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the connection for the given name.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::DB::Connection* ConnectionPool::getConnection ( const std::string& name )
{
  return _connections[name].get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a connection.
//
///////////////////////////////////////////////////////////////////////////////

void ConnectionPool::addConnection ( Minerva::Core::DB::Connection* connection )
{
  _connections[ connection->name() ] = connection;
}

