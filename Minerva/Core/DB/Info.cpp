
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/DB/Info.h"

using namespace Minerva::Core::DB;


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
  std::string query ( "SELECT table_name FROM information_schema.tables WHERE table_schema='public'" );
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
    if( this->isLineTable( *iter ) )
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
  std::string query ( "SELECT table_name FROM information_schema.columns WHERE column_name='geom'" );
  return this->_fillStringsFromQuery( query );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the geometry type for a table.
//
///////////////////////////////////////////////////////////////////////////////

std::string Info::_getGeometryType ( const std::string& table )
{
  std::string query ( "SELECT GeometryType(geom) FROM " + table + " WHERE IsEmpty(geom)=false LIMIT 1" );
  pqxx::result result ( _connection->executeQuery ( query ) );

  if( false == result.empty() )
    return result[0][0].as < std::string > ();

  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy results of a query into a vector.
//
///////////////////////////////////////////////////////////////////////////////

Info::Strings Info::_fillStringsFromQuery( const std::string& query )
{
  Strings strings;

  pqxx::result result ( _connection->executeQuery ( query ) );
  for( pqxx::result::const_iterator iter = result.begin(); iter != result.end(); ++iter )
  {
    strings.push_back( iter[0].as < std::string > () );
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
  std::string query ( "SELECT GeometryType(geom) FROM " + table + " WHERE IsEmpty(geom)=false LIMIT 1" );
  pqxx::result result ( _connection->executeQuery ( query ) );

  if( false == result.empty() )
  {
    std::string type ( result[0][0].as < std::string > () );
    return ( ( type == "POINT" ) || ( type == "MULTIPOINT" ) );
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given table a line table?
//
///////////////////////////////////////////////////////////////////////////////

bool Info::isLineTable ( const std::string& table ) const
{
  std::string query ( "SELECT GeometryType(geom) FROM " + table + " WHERE IsEmpty(geom)=false LIMIT 1" );
  pqxx::result result ( _connection->executeQuery ( query ) );

  if( false == result.empty() )
  {
    std::string type ( result[0][0].as < std::string > () );
    return ( ( type == "LINE" ) || ( type == "MULTILINE" ) );
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given table a polygon table?
//
///////////////////////////////////////////////////////////////////////////////

bool Info::isPolygonTable ( const std::string& table ) const
{
  std::string query ( "SELECT GeometryType(geom) FROM " + table + " WHERE IsEmpty(geom)=false LIMIT 1" );
  pqxx::result result ( _connection->executeQuery ( query ) );

  if( false == result.empty() )
  {
    std::string type ( result[0][0].as < std::string > () );
    return ( ( type == "POLYGON" ) || ( type == "MULTIPOLYGON" ) );
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the table have a column of the given type?
//
///////////////////////////////////////////////////////////////////////////////

bool Info::hasColumnType ( const std::string& table, const std::string& type ) const
{
  std::string query ( "SELECT column_name FROM information_schema.columns WHERE table_name='" + table + "' AND data_type='" + type + "'" );
  pqxx::result result ( _connection->executeQuery( query ) );
  return !result.empty();
}
