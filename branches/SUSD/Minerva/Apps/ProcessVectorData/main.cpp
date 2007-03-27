
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Decision Theater at Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////


#include "Minerva/DB/Connection.h"

#include "Threads/OpenThreads/Mutex.h"

#include "Usul/App/Controller.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Components/Manager.h"

#include "Usul/Policies/Update.h"

#include "osgDB/WriteFile"

#include "boost/program_options/options_description.hpp"
#include "boost/program_options/variables_map.hpp"
#include "boost/program_options/parsers.hpp"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////
//
//  Set for multi-threaded. This is global because the sooner we set this, 
//  the better. Setting in main() may be too late.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // Set command-line arguments.
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  std::string session ( "" );

  boost::program_options::options_description desc ( "Allowed Options" );
  desc.add_options()
    ( "help", "Show this message." )
    ( "server,s", "Name of server (host) where database resides." )
    ( "database,d", "Database to connect to." )
    ( "user,u", "Username" )
    ( "password,p", "Password" )
    ( "tablename,t", "Table to process" )
  ;

  boost::program_options::variables_map vm;
  boost::program_options::store ( boost::program_options::parse_command_line( argc, argv, desc ), vm );
  boost::program_options::notify ( vm );

  if( vm.count( "help" ) )
  {
    std::cout << desc << std::endl;
    return 1;
  }

  std::string hostname, database, password, username, tablename;

  if( vm.count( "server" ) )
    hostname = vm[ "server" ].as < std::string > ();
  else
  {
    std::cout << desc << std::endl;
    return 1;
  }

  if( vm.count( "database" ) )
    database = vm[ "database" ].as  < std::string > ();
  else
  {
    std::cout << desc << std::endl;
    return 1;
  }

  if( vm.count( "user" ) )
    username = vm[ "user" ].as < std::string > ();
  else
  {
    std::cout << desc << std::endl;
    return 1;
  }

  if( vm.count( "password" ) )
    password = vm[ "password" ].as < std::string > ();
  else
  {
    std::cout << desc << std::endl;
    return 1;
  }

  if( vm.count ( "tablename" ) )
    tablename = vm[ "tablename" ].as < std::string > ();
  else
  {
    std::cout << desc << std::endl;
    return 1;
  }

  Minerva::DB::Connection::RefPtr applicationConnection ( new Minerva::DB::Connection );
  applicationConnection->username( username );
  applicationConnection->password( password );
  applicationConnection->database( database );
  applicationConnection->hostname( hostname );
  applicationConnection->connect();


  return 0;
}
