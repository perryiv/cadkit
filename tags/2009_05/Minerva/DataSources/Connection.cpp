
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/DataSources/Connection.h"

using namespace Minerva::DataSources;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Connection::Connection() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Connection::~Connection()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute an insert query.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::executeInsertQuery ( const std::string &tableName, const Values &namesAndValues )
{
  if ( namesAndValues.size() == 0)
    throw std::runtime_error ("Error 1978842812: No names or values in the query");

  std::string names ( "( " );
  std::string values ( " VALUES ( " );

  for ( Values::const_iterator iter = namesAndValues.begin(); iter != namesAndValues.end(); ++iter )
  {
    names += iter->first;

    values += " '" + iter->second + "' ";

    if (iter != namesAndValues.end() - 1)
    {
      names += ",";
      values += ",";
    }
  }

  names += " )";
  values += " )";

  std::string query ( "INSERT INTO " + tableName + names + values );

  this->executeQuery ( query );
}
