
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/DataSources/PG/Info.h"

using namespace Minerva::DataSources::PG;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Info::Info( Connection* connection ) : 
  BaseClass(),
  _connection ( connection )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Info::~Info()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all public tables.
//
///////////////////////////////////////////////////////////////////////////////

Info::Strings Info::tables()
{
  std::string query ( "SELECT table_schema || '.' || table_name FROM information_schema.tables" );
  return this->_fillStringsFromQuery( query );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all polygon tables.
//
///////////////////////////////////////////////////////////////////////////////

Info::Strings Info::polygonTables()
{
  Strings strings;

  Strings geometryTables ( this->geometryTables() );

  for( Strings::const_iterator iter = geometryTables.begin(); iter != geometryTables.end(); ++iter )
  {
    if( this->isPolygonTable( *iter ) )
      strings.push_back( *iter );
  }

  return strings;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all line tables.
//
///////////////////////////////////////////////////////////////////////////////

Info::Strings Info::lineTables()
{
  Strings strings;

  Strings geometryTables ( this->geometryTables() );

  for( Strings::const_iterator iter = geometryTables.begin(); iter != geometryTables.end(); ++iter )
  {
    if( this->isLineTable( *iter ) || this->isPolygonTable ( *iter ) )
      strings.push_back( *iter );
  }

  return strings;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all point tables.
//
///////////////////////////////////////////////////////////////////////////////

Info::Strings Info::pointTables()
{
  Strings strings;

  Strings geometryTables ( this->geometryTables() );

  for( Strings::const_iterator iter = geometryTables.begin(); iter != geometryTables.end(); ++iter )
  {
    if( this->isPointTable( *iter ) )
      strings.push_back( *iter );
  }

  return strings;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all point time tables.
//
///////////////////////////////////////////////////////////////////////////////

Info::Strings Info::pointTimeTables()
{
  Strings strings;

  Strings pointTables ( this->pointTables() );

  for( Strings::const_iterator iter = pointTables.begin(); iter != pointTables.end(); ++iter )
  {
    if( this->hasColumnType( *iter, "date" ) )
      strings.push_back( *iter );
  }

  return strings; 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all geometry tables.
//
///////////////////////////////////////////////////////////////////////////////

Info::Strings Info::geometryTables()
{
  std::string query ( "SELECT table_schema || '.' || table_name FROM information_schema.columns WHERE column_name='geom'" );
  return this->_fillStringsFromQuery( query );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the geometry type for a table.
//
///////////////////////////////////////////////////////////////////////////////

std::string Info::_getGeometryType ( const std::string& table ) const
{
  std::string query ( "SELECT GeometryType(geom) FROM " + table + " WHERE IsEmpty(geom)=false LIMIT 1" );
  Minerva::DataSources::Result::RefPtr result ( _connection->executeQuery ( query ) );

  if( result->prepareNextRow() )
    return result->asString ( 0 );

  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy results of a query into a vector.
//
///////////////////////////////////////////////////////////////////////////////

Info::Strings Info::_fillStringsFromQuery( const std::string& query ) const
{
  Strings strings;

  Minerva::DataSources::Result::RefPtr result ( _connection->executeQuery ( query ) );
  while ( result->prepareNextRow() )
  {
    strings.push_back ( result->asString ( 0 ) );
  }

  return strings;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given table a point table?
//
///////////////////////////////////////////////////////////////////////////////

bool Info::isPointTable ( const std::string& table ) const
{
  const std::string type ( this->_getGeometryType ( table ) );
  return ( ( type == "POINT" ) || ( type == "MULTIPOINT" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given table a point time table?
//
///////////////////////////////////////////////////////////////////////////////

bool Info::isPointTimeTable ( const std::string& table ) const
{
  return ( this->isPointTable( table ) && this->hasColumnType( table, "date" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given table a line table?
//
///////////////////////////////////////////////////////////////////////////////

bool Info::isLineTable ( const std::string& table ) const
{
  const std::string type ( this->_getGeometryType ( table ) );
  return ( ( type == "LINESTRING" ) || ( type == "MULTILINESTRING" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given table a polygon table?
//
///////////////////////////////////////////////////////////////////////////////

bool Info::isPolygonTable ( const std::string& table ) const
{
  const std::string type ( this->_getGeometryType ( table ) );
  return ( ( type == "POLYGON" ) || ( type == "MULTIPOLYGON" ) );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given table a point time table?
//
///////////////////////////////////////////////////////////////////////////////

bool Info::isPolygonTimeTable ( const std::string& table ) const
{
  return ( this->isPolygonTable( table ) && this->hasColumnType( table, "date" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the schema name if present.
//  Use this function when querying the information_schema table.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail 
{
  std::string tableName ( const std::string& table )
  {
    // Make a copy.
    std::string name ( table );

    // See if there is a schema specified.
    std::string::size_type pos ( name.find_first_of ( '.' ) );
    
    // Trim the table if we should.
    if( std::string::npos != pos )
      name = table.substr ( pos + 1, name.length() - pos );

    return name;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Does the table have a column of the given type?
//
///////////////////////////////////////////////////////////////////////////////

bool Info::hasColumnType ( const std::string& table, const std::string& type ) const
{
  // Remove the schema name, if any.
  std::string name ( Detail::tableName( table ) );

  // Run the query.
  std::string query ( "SELECT column_name FROM information_schema.columns WHERE table_name='" + name + "' AND data_type='" + type + "'" );
  Minerva::DataSources::Result::RefPtr result ( _connection->executeQuery ( query ) );
  return !result->empty();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all column names.
//
///////////////////////////////////////////////////////////////////////////////

Info::Strings Info::getColumnNames ( const std::string& table ) const
{
  // Remove the schema name, if any.
  std::string name ( Detail::tableName( table ) );

  // Run the query.
  std::string query ( "SELECT column_name FROM information_schema.columns WHERE table_name='" + name + "'" );
  return _fillStringsFromQuery( query );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all column names.
//
///////////////////////////////////////////////////////////////////////////////

Info::Strings Info::getColumnNames ( const std::string& table, const std::string& type ) const
{
  // Remove the schema name, if any.
  std::string name ( Detail::tableName( table ) );

  // Run the query.
  std::string query ( "SELECT column_name FROM information_schema.columns WHERE table_name='" + name + "' AND data_type='" + type + "'" );
  return _fillStringsFromQuery( query );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get min and max value for given query.
//
///////////////////////////////////////////////////////////////////////////////

void Info::getMinMaxValue( const std::string& query, const std::string& columnName, double& min, double& max ) const
{
  std::string q ( "SELECT MAX(" + columnName + ") as max, MIN(" + columnName + ") FROM (" + query + ") temp" );

  Minerva::DataSources::Result::RefPtr result ( _connection->executeQuery( q ) );
  if( result->prepareNextRow() )
  {
    min = result->asDouble ( "min" );
    max = result->asDouble ( "max" );
  }
}
